#pragma once
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <windows.h>
#include <curl/curl.h>
#include <thread>
#include <atomic>
#include <mutex>
#include <sstream>
#include <condition_variable>

class DatabaseManager
{
public:
	DatabaseManager(const DatabaseManager&) = delete;
	void operator=(const DatabaseManager&) = delete;

	DatabaseManager() {}
	~DatabaseManager() {}

	static DatabaseManager* GetInstance()
	{
		static DatabaseManager instance;
		return &instance;
	}

	// Custom app-level message for notifying main thread of new Firebase data
	static const UINT WM_FIREBASE_UPDATE = WM_APP + 101;

	// Forward declarations
	void StartFirebaseStream();
	void StopFirebaseStream();
	void ApplyFirebaseStreamData(); // runs on UI thread (main window)
	static size_t FirebaseStreamWriteCallback(void *contents, size_t size, size_t nmemb, void *userp);

};