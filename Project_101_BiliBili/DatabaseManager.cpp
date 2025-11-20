#include "DatabaseManager.h"
#include "App.h"
#include "json.hpp"

using json = nlohmann::json;

// Thread/control state for Firebase streaming
static std::thread g_firebaseThread;
static std::atomic<bool> g_firebaseRunning{ false };
static std::mutex g_firebaseMutex;
static std::string g_firebaseLatestPayload; // JSON string protected by mutex

// Start the streaming thread; safe to call multiple times (will guard)
void DatabaseManager::StartFirebaseStream()
{
	if (g_firebaseRunning.load()) return; // already running

	g_firebaseRunning.store(true);
	g_firebaseThread = std::thread([]()
		{
			// thread-local buffer and CURL handle
			CURL *curl = nullptr;
			CURLcode res = CURLE_OK;

			while (g_firebaseRunning.load())
			{
				curl = curl_easy_init();
				if (!curl)
				{
					// Failed to init curl; back off a bit and retry
					std::this_thread::sleep_for(std::chrono::seconds(1));
					continue;
				}

				// Set the REST streaming endpoint (Firebase Realtime DB)
				// Keep the same endpoint you used before
				const char *url = "https://bilibili-9e370-default-rtdb.asia-southeast1.firebasedatabase.app/parameters.json";

				curl_easy_setopt(curl, CURLOPT_URL, url);
				// Request server-sent events stream
				struct curl_slist *headers = nullptr;

				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0);
				curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0);

				// Windows: avoid signals in libcurl
				curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
				headers = curl_slist_append(headers, "Accept: text/event-stream");
				headers = curl_slist_append(headers, "Connection: keep-alive");
				curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
				curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, FirebaseStreamWriteCallback);
				// userp not used; callback will use global/mutex & PostMessage
				curl_easy_setopt(curl, CURLOPT_WRITEDATA, nullptr);

				// Keepalive options (optional)
				curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);
				curl_easy_setopt(curl, CURLOPT_TCP_KEEPIDLE, 60L);
				curl_easy_setopt(curl, CURLOPT_TCP_KEEPINTVL, 30L);

				// inside StartFirebaseStream, after curl_easy_init() and before curl_easy_perform(curl);
				curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, 5L);      // fail connect quickly if unreachable
				curl_easy_setopt(curl, CURLOPT_TIMEOUT, 15L);            // ensure perform returns periodically (seconds)

				// for debug
				curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

				// Make the blocking streaming call. It will return on error or when server closes.
				res = curl_easy_perform(curl);
				if (res != CURLE_OK)
				{
					std::fprintf(stderr, "Firebase stream curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
				}

				// Clean up this attempt
				curl_slist_free_all(headers);
				curl_easy_cleanup(curl);
				curl = nullptr;

				// If still running, wait and then reconnect (simple backoff)
				if (g_firebaseRunning.load())
				{
					std::this_thread::sleep_for(std::chrono::milliseconds(500));
				}
			}
		}); // end thread lambda
}

// Stop streaming and join the thread
void DatabaseManager::StopFirebaseStream()
{
	if (!g_firebaseRunning.load()) return;

	g_firebaseRunning.store(false);
	// curl_easy_perform will eventually return (server close or error). Wait for thread to join.
	if (g_firebaseThread.joinable())
	{
		// give it some time to exit gracefully, but then join
		g_firebaseThread.join();
		curl_global_cleanup();
	}
}

// libcurl write callback for SSE; runs in background thread
size_t DatabaseManager::FirebaseStreamWriteCallback(void *contents, size_t size, size_t nmemb, void * /*userp*/)
{
	// if stop requested, tell libcurl to abort by returning a short write result (0 triggers CURLE_WRITE_ERROR)
	if (!g_firebaseRunning.load())
		return 0;

	size_t total = size * nmemb;
	thread_local std::string buffer;
	buffer.append(static_cast<char *>(contents), total);

	// Normalize CRLF -> LF so we can reliably split on "\n\n"
	size_t pos_replace = 0;
	while ((pos_replace = buffer.find("\r\n", pos_replace)) != std::string::npos)
	{
		buffer.replace(pos_replace, 2, "\n");
	}

	// Process completed SSE events separated by blank line ("\n\n")
	for (;;)
	{
		size_t sep = buffer.find("\n\n");
		if (sep == std::string::npos) break;

		std::string eventBlock = buffer.substr(0, sep);
		buffer.erase(0, sep + 2);

		std::istringstream iss(eventBlock);
		std::string line;
		std::string dataPayload;
		while (std::getline(iss, line))
		{
			// Remove possible trailing CR (defensive)
			if (!line.empty() && line.back() == '\r')
				line.pop_back();

			const std::string dataPrefix = "data:";
			if (line.size() >= dataPrefix.size() && line.compare(0, dataPrefix.size(), dataPrefix) == 0)
			{
				std::string part = line.substr(dataPrefix.size());
				// trim leading whitespace
				while (!part.empty() && (part.front() == ' ' || part.front() == '\t'))
					part.erase(part.begin());
				dataPayload += part;
			}
		}

		if (!dataPayload.empty())
		{
			{
				std::lock_guard<std::mutex> lock(g_firebaseMutex);
				g_firebaseLatestPayload = dataPayload;
			}

			App *app = App::GetInstance();
			if (app && app->hwnd)
			{
				PostMessage(app->hwnd, DatabaseManager::WM_FIREBASE_UPDATE, 0, 0);
			}
		}
	}

	return total;
}

// Called on UI thread. Grabs latest payload under lock and applies parameter updates to UI controls.
void DatabaseManager::ApplyFirebaseStreamData()
{
	std::string payload;
	{
		std::lock_guard<std::mutex> lock(g_firebaseMutex);
		if (g_firebaseLatestPayload.empty()) return;
		payload = g_firebaseLatestPayload;
		// optional: clear so same payload not reapplied repeatedly
		// g_firebaseLatestPayload.clear();
	}

	json data;
	try
	{
		data = json::parse(payload);
	}
	catch (const std::exception &ex)
	{
		std::fprintf(stderr, "Failed to parse Firebase payload JSON: %s\n", ex.what());
		return;
	}

	if (!data.contains("data"))
	{
		std::fprintf(stderr, "Firebase payload missing 'data' field\n");
		return;
	}

	// Normalize into an object mapping param-name -> value
	json root;
	if (data["data"].is_object())
	{
		// initial full payload: data is object
		root = data["data"];
	}
	else
	{
		// update payload: data is a primitive, use path to map key
		if (!data.contains("path") || !data["path"].is_string())
		{
			std::fprintf(stderr, "Firebase payload primitive with no valid 'path'\n");
			return;
		}

		std::string path = data["path"].get<std::string>();
		// strip leading slashes
		while (!path.empty() && path.front() == '/') path.erase(path.begin());
		if (path.empty())
		{
			// primitive at root; skip
			std::fprintf(stderr, "Firebase primitive at root path; ignoring\n");
			return;
		}

		// use last segment as key
		size_t pos = path.find_last_of('/');
		std::string key = (pos == std::string::npos) ? path : path.substr(pos + 1);

		root = json::object();
		root[key] = data["data"];
	}

	auto &toolbar = App::GetInstance()->toolbar;
	//auto params = reinterpret_cast<std::vector<ParamUI>*>(GetWindowLongPtr(toolbar.hToolbar, GWLP_USERDATA));
	//if (!params) return;

	// Update App parameters and UI sliders/texts on the main thread
	for (size_t i = 0; i < toolbar.parameters.size(); ++i)
	{
		auto &param = toolbar.parameters[i];
		if (root.contains(param.name))
		{
			try
			{
				float newVal = root[param.name].get<float>() * param.divisionBy;
				param.SetValue(newVal);
			}
			catch (const std::exception &ex)
			{
				std::fprintf(stderr, "Error applying parameter '%s': %s\n", param.name.c_str(), ex.what());
			}
		}
	}
}