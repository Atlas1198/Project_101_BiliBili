#include "App.h"
#include <algorithm>
#include <mmsystem.h>
#include <tchar.h>
#include "Keyboard.h"
#include "resource.h"
#include <commctrl.h>
#include <commdlg.h>
#include "json.hpp"
#include <fstream>
#include "EventManager.h"
#include "AudioResources.h"
#include "ContentRoot.h"
#include "Debug.h"
 
using json = nlohmann::json;

#pragma comment(lib, "winmm.lib")

void InitializeDPIScale(HWND hwnd);
bool LoadParametersJSON();

namespace
{
void ReportStartupError(HWND owner, const wchar_t* message, const char* debugMessage)
{
	OutputDebugStringA(debugMessage);
	MessageBoxW(owner, message, L"BiliBili - Startup Error", MB_OK | MB_ICONERROR);
}

void ReportAudioWarning(HWND owner)
{
	MessageBoxW(owner, L"Audio could not be initialized. The game will continue without sound.",
		L"BiliBili - Audio Warning", MB_OK | MB_ICONWARNING);
}
}

//ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:
	{
		InitializeDPIScale(hwnd);

		// Get the monitor that the window is currently mostly resting on
		HMONITOR hMonitor = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
		MONITORINFO mi = { sizeof(mi) };
		GetMonitorInfo(hMonitor, &mi);

		// Set borderless window style
		SetWindowLong(hwnd, GWL_STYLE, WS_POPUP | WS_VISIBLE);

		// Resize and position the window to cover the entire monitor
		SetWindowPos(hwnd, HWND_TOP,
			mi.rcMonitor.left,
			mi.rcMonitor.top,
			mi.rcMonitor.right - mi.rcMonitor.left,
			mi.rcMonitor.bottom - mi.rcMonitor.top,
			SWP_NOOWNERZORDER | SWP_FRAMECHANGED);

		break;
	}
	case WM_ACTIVATEAPP:	//アクティブウィンドウが切り替わった
	case WM_SYSKEYDOWN:		//システムキーが押された
	case WM_KEYUP:			//キーが離された
	case WM_SYSKEYUP:		//システムキーが離された
		//キーボードメッセージ処理
		Keyboard_ProcessMessage(msg, wParam, lParam);
		break;

	case WM_KEYDOWN:		//キーが押された
		if (wParam == VK_ESCAPE)//押されたのはESCキー
		{
			//ウィンドウを閉じたいリクエストをWindowsに送る
			SendMessage(hwnd, WM_CLOSE, 0, 0);
		}

		//キーボードメッセージ処理
		Keyboard_ProcessMessage(msg, wParam, lParam);
		break;

	case WM_DESTROY:		//ウィンドウ破壊時
		PostQuitMessage(0);	//OSに対する終了メッセージ
		return 0;
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}

//シングルトンインスタンス取得
App* App::GetInstance()
{
	static App instance; //シングルトンインスタンス
	return &instance;
}

//初期化
bool App::Initialize()
{
	if (!ContentRoot::GetInstance()->Initialize())
	{
		ReportStartupError(nullptr, L"The asset folder could not be found.", "[App] Asset folder not found\n");
		return false;
	}

	CreateMainWindow(hwnd, wc);	//メインウィンドウの生成
	if (!hwnd)
	{
		ReportStartupError(nullptr, L"Failed to create the main window.", "[App] Failed to create the main window\n");
		return false;
	}

	PrepareInstance(); // インスタンス準備

	if (!InitInstance())
	{
		ReportStartupError(hwnd, L"The game engine could not be initialized.", "[App] Engine initialization failed\n");
		Terminate();
		return false;
	}
	return true;
}

//実行
void App::Run()
{
	//ウィンドウの表示
	ShowWindow(hwnd, SW_SHOW);	

	//メッセージループ
	MSG msg = {};	//メッセージ

	//フレームレート計測用変数
	DWORD	dwExecLastTime;	//前回の実行時間
	DWORD	dwFPSLastTime;	//前回のFPS計測時間
	DWORD	dwCurrentTime;	//現在の時間
	DWORD	dwFrameCount;	//フレームカウント

#ifdef _DEBUG	//デバッグビルド時のみFPS表示
	int		countFPS = {};		//FPSカウンター
	char	debugStr[2048];	//FPS表示文字列
#endif

	//フレームレート計測初期化
	timeBeginPeriod(1);								//タイマーの精度を設定
	dwExecLastTime = dwFPSLastTime = timeGetTime();	//現在のタイマー値
	dwCurrentTime = dwFrameCount = 0;				//初期化

	players.clear();

	/*int msgboxID = MessageBox(
		NULL,
		"オンラインモードに入りますか？",
		"モード選択",
		MB_ICONQUESTION | MB_YESNO
	);*/

	/*if (msgboxID == IDYES)
	{
		LoadParametersJSON();
		UpdateParameters();
		isOnline = true;
		if (!Login()) return;
	}*/
	isOnline = false;

	m_pSceneManager->SpawnPlayers();

	LoadParametersJSON();
	UpdateParameters();

	do 
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{//メッセージがある場合
			TranslateMessage(&msg);	//メッセージの変換
			DispatchMessage(&msg);	//メッセージの送出
		}
		else
		{//メッセージが無い場合
			dwCurrentTime = timeGetTime();	//現在のタイマー値取得

			if ((dwCurrentTime - dwFPSLastTime) >= 1000)
			{
#ifdef _DEBUG
				countFPS = dwFrameCount;	//FPSカウント保存
#endif
				dwFPSLastTime = dwCurrentTime;	//現在のタイマー値保存
				dwFrameCount = 0;				//フレームカウント初期化
			}

			if ((dwCurrentTime - dwExecLastTime) >= ((float)1000 / 60))
			{//60FPSで動作させる
				dwExecLastTime = dwCurrentTime;	//現在のタイマーと保存
				dwFrameCount++;					//フレームカウントアップ
#ifdef _DEBUG
				//ウィンドウキャプションへ現在のFPSを表示
				wsprintf(debugStr, "Project101 ");	//ウィンドウタイトル文字列
				wsprintf(
					&debugStr[strlen(debugStr)],	//文字列連結
					" FPS : %d", countFPS			//FPS
				);
				SetWindowText(hwnd, debugStr);	//ウィンドウタイトルの設定
#endif
				UpdateParameters();

				ReadMessages();
				//更新処理
				Update();	//更新
				WriteMessages();

				//描画処理
				Draw();		//描画

				//フレーム終了後処理
				m_pInputManager->Copy(); //入力管理クラスのキー情報コピー
			}
		}
	} while (msg.message != WM_QUIT);	//終了メッセージが来るまで
}

//終了
void App::Terminate()
{
	m_pEventManager = nullptr;
	if (m_pAudioManager)
	{
		m_pAudioManager->StopAll();
		m_pAudioManager = nullptr;
	}

	if (m_pSceneManager)
	{
		m_pSceneManager->Finalize();
		delete m_pSceneManager;
		m_pSceneManager = nullptr;
	}
	delete m_pRenderer;
	m_pRenderer = nullptr;
	delete m_pMeshManager;
	m_pMeshManager = nullptr;
	delete m_pTextureManager;
	m_pTextureManager = nullptr;
	delete m_pInputManager;
	m_pInputManager = nullptr;

	if (m_pEngine)
	{
		m_pEngine->Terminate();
		delete m_pEngine;
		m_pEngine = nullptr;
	}

	if (hwnd && IsWindow(hwnd))
	{
		DestroyWindow(hwnd);
	}
	hwnd = nullptr;

	UnregisterClass(wc.lpszClassName, wc.hInstance);	//ウィンドウクラスの登録解除
}

//メインウィンドウの生成
void App::CreateMainWindow(HWND& hwnd, WNDCLASSEX& wc)
{
	HINSTANCE hInst = GetModuleHandle(nullptr);
	//ウィンドウクラス生成＆登録
	wc.cbSize = sizeof(WNDCLASSEX);							//構造体のサイズ
	wc.style = CS_HREDRAW | CS_VREDRAW;						//スタイル
	wc.lpfnWndProc = (WNDPROC)WindowProcedure;				//コールバック関数の指定
	wc.hIcon = LoadIcon(hInstance, IDI_APPLICATION);		//アイコンの指定
	wc.hCursor = LoadCursor(hInstance, IDC_ARROW);			//カーソルの指定
	wc.hbrBackground = GetSysColorBrush(COLOR_BACKGROUND);	//背景色の指定
	wc.lpszMenuName = nullptr;								//メニューの指定
	wc.lpszClassName = _T("101_engine");					//クラス名の指定
	wc.hInstance = GetModuleHandle(NULL);					//インスタンスハンドルの指定

	//ウィンドウクラスの登録
	RegisterClassEx(&wc);

	//RECT wrc = { 0,0, WINDOW_WIDTH, WINDOW_HEIGHT };//ウィンドウサイズを決める
	RECT rect;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rect, 0);
	//ウィンドウサイズを補正
	//AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindowEx(
		0,						// 拡張スタイル
		wc.lpszClassName,		//クラス名の指定
		_T("DX12_Application"),	//タイトルバーの文字
		WS_POPUP | WS_VISIBLE,	//ウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標はOSにお任せします
		CW_USEDEFAULT,			//表示Y座標はOSにお任せします
		rect.right - rect.left,	//ウィンドウ横幅
		rect.bottom - rect.top,	//ウィンドウ縦幅
		NULL,					//親ウィンドウハンドル
		NULL,					//メニューハンドル
		hInstance,				//インスタンスハンドル
		NULL					//オプション
	);
}

void App::PrepareInstance()
{
	m_pEngine = new Engine();					//DirectX12エンジンの生成
	m_pRenderer = new Renderer();				//レンダラーの生成
	m_pInputManager = new InputManager();		//入力管理クラスの生成
	m_pSceneManager = new SceneManager(			//シーン管理クラスの生成
		static_cast<float>(WINDOW_WIDTH),
		static_cast<float>(WINDOW_HEIGHT)
	);
	m_pTextureManager = new TextureManager();	//テクスチャ管理クラスの生成
	m_pMeshManager = new MeshManager();			//メッシュ管理クラスの生成
	m_pAudioManager = AudioManager::GetInstance();		//オーディオ管理クラスの生成

}

//インスタンス初期化
bool App::InitInstance()
{
	m_pEventManager = EventManager::GetInstance();

	//DirectX12エンジン初期化
	if (!m_pEngine->InitCore(
		hwnd,			//ウィンドウハンドル
		WINDOW_WIDTH,	//フレームバッファの幅
		WINDOW_HEIGHT	//フレームバッファの高さ
	)) return false;

	//デバイスの取得
	auto pDevice = m_pEngine->GetDevice();

	//レンダラー初期化
	//テクスチャ管理クラス初期化
	if (!m_pTextureManager->Initialize(
		pDevice,	//デバイス
		1024		//最大ディスクリプタ数
	)) return false;

	//メッシュ管理クラス初期化
	if (!m_pMeshManager->Initialize(
		pDevice	//デバイス
	)) return false;

	if (!m_pRenderer->Initialize(
		pDevice,
		m_pSceneManager->GetCameraInfo(),
		m_pTextureManager
	)) return false;

	//バインディングの初期化
	if (!m_pEngine->InitBindings(m_pTextureManager)) return false;

	//レンダーを開始してコマンドリストをオープン
	m_pEngine->BeginFrame();	

	m_pInputManager->Initialize();
  
	//オーディオ管理クラス初期化
	if (m_pAudioManager)
	{
		if (m_pAudioManager->Initialize())
		{
			LoadAllGameSounds(*m_pAudioManager);	//カタログ関数を呼ぶ
		}
		else
		{
			ReportAudioWarning(hwnd);
		}
	}

	//シーン管理クラス初期化
	m_pSceneManager->Initialize(
		m_pInputManager,	//入力管理クラス
		m_pTextureManager,	//テクスチャ管理クラス
		m_pMeshManager		//メッシュ管理クラス
	);

	//レンダーを終了してコマンドリストをクローズ
	if (!m_pEngine->RenderEnd()) return false;
	return true;
}

//更新
void App::Update()
{
	if (isOnline && waitingForConnection) return;
	//現在のバックバッファインデックスを取得
	const UINT backIdx = m_pEngine->GetCurrentBufferIndex();

	//各種更新処理
	m_pRenderer->BeginFrame(backIdx);	//フレーム開始（内部キューをクリア）

	if (!isGameOver)
	{
		m_pInputManager->Update();			//入力管理クラスの更新
		m_pSceneManager->Update();			//ゲームの更新
	}

	if (m_pAudioManager) { m_pAudioManager->Update(); }	//オーディオの更新

	m_pRenderer->Update(backIdx, *m_pSceneManager->GetCameraInfo());		//レンダラーの更新
}

//描画
void App::Draw()
{
	// Start rendering
	m_pEngine->BeginFrame();

	// Upload pending textures
	m_pTextureManager->UploadPendingTextures(m_pEngine->GetCommandList());

	// Submit draw requests for the game scene
	m_pSceneManager->SubmitDraws(*m_pRenderer);

	// Draw for post-processing
	m_pEngine->BeginPass(RENDER_TARGET_TYPE::POST_PROCESS);
	m_pRenderer->Draw(m_pEngine->GetCommandList(), RENDER_TARGET_TYPE::POST_PROCESS);
	m_pEngine->EndPass(RENDER_TARGET_TYPE::POST_PROCESS);

	// Draw for back buffer
	auto backBufferType = static_cast<RENDER_TARGET_TYPE>(m_pEngine->GetCurrentBufferIndex());
	m_pEngine->BeginPass(backBufferType);
	m_pRenderer->Draw(m_pEngine->GetCommandList(), backBufferType);
	m_pEngine->EndPass(backBufferType);

	// End rendering
	m_pEngine->RenderEnd();
}

bool App::Login()
{
	if (Connect("127.0.0.1", 60000))
	{
		return true;
	}

	return false;
}

void App::ReadMessages()
{
	if (!isOnline) return;
	// Check for incoming network messages
	if (IsConnected())
	{
		while (!Incoming().empty())
		{
			auto msg = Incoming().pop_front().msg;

			switch (msg.header.id)
			{
				case(GameMsg::Client_Accepted):
				{
					std::cout << "Server accepted client - you're in!\n";

					int gameID = 0;
					msg >> gameID;
					if (!msg.is_valid())
					{
						DebugLogA("[Network] Invalid Client_Accepted message was ignored\n");
						break;
					}

					olc::net::message<GameMsg> msg;
					msg.header.id = GameMsg::Client_RegisterWithServer;

					descPlayer.ingameID = gameID;
					descPlayer.pos = spawnPos;
					//descPlayer.radius = 0.1f;

					msg << descPlayer;
					Send(msg);
					break;
				}

				case(GameMsg::Client_AssignID):
				{
					// Server is assigning us OUR id
					msg >> descPlayer.uniqueID;
					if (!msg.is_valid())
					{
						DebugLogA("[Network] Invalid Client_AssignID message was ignored\n");
						break;
					}
					std::cout << "Assigned Client ID = " << descPlayer.uniqueID << "\n";
					break;
				}

				case(GameMsg::Game_AddPlayer):
				{
					PlayerDescription newDesc;

					msg >> newDesc >> playerCount;
					if (!msg.is_valid())
					{
						DebugLogA("[Network] Invalid Game_AddPlayer message was ignored\n");
						break;
					}

					players.insert_or_assign(newDesc.uniqueID, newDesc);

					std::cout << "Assigned Client ID = " << descPlayer.uniqueID << "\n";

					if (newDesc.uniqueID == descPlayer.uniqueID)
					{
						// Now we exist in game world
						waitingForConnection = false;
					}

					m_pSceneManager->AddPlayer(newDesc.uniqueID);

					break;
				}

				case(GameMsg::Game_RemovePlayer):
				{
					uint32_t nRemovalID = 0;
					msg >> nRemovalID >> playerCount;
					if (!msg.is_valid())
					{
						DebugLogA("[Network] Invalid Game_RemovePlayer message was ignored\n");
						break;
					}
					players.erase(nRemovalID);
					m_pSceneManager->RemovePlayer(nRemovalID);
					break;
				}

				case(GameMsg::Game_UpdatePlayer):
				{
					PlayerDescription desc;
					msg >> desc;
					if (!msg.is_valid())
					{
						DebugLogA("[Network] Invalid Game_UpdatePlayer message was ignored\n");
						break;
					}
					players.insert_or_assign(desc.uniqueID, desc);
					break;
				}
			}
		}
	}
}

void App::WriteMessages()
{
	if (!isOnline) return;
	// Send player description
	if (!waitingForConnection)
	{
		olc::net::message<GameMsg> msg;
		msg.header.id = GameMsg::Game_UpdatePlayer;
		msg << players[descPlayer.uniqueID];
		Send(msg);
	}
}

void App::UpdateParameters()
{
	constexpr size_t requiredParameterCount = 7;
	if (toolbar.parameters.size() < requiredParameterCount)
	{
		static bool warningLogged = false;
		if (!warningLogged)
		{
			OutputDebugStringA("[Parameters] Insufficient toolbar parameters; current values will be kept\n");
			warningLogged = true;
		}
		return;
	}

	Player::MOVE_SPEED = toolbar.parameters[0].GetValue();
	Player::BULLET_SPEED = toolbar.parameters[1].GetValue();
	BulletManager::BULLET_RECOVERY = toolbar.parameters[2].GetValue();
	BulletManager::BULLET_DAMAGE = toolbar.parameters[3].GetValue();
	ItemManager::ITEM_RESPAWN = toolbar.parameters[4].GetValue();
	BBManager::BB_DURATION = toolbar.parameters[5].GetValue();
	BB::DAMAGE = toolbar.parameters[6].GetValue();
}

void InitializeDPIScale(HWND hwnd)
{
	const UINT dpi = GetDpiForWindow(hwnd);
	App::DPIScale = static_cast<float>(dpi) / USER_DEFAULT_SCREEN_DPI;
}

bool LoadParametersJSON()
{
	const auto path = ContentRoot::GetInstance()->ResolveAsset(L"defaults.json");
	std::ifstream f(path);
	if (!f)
	{
		DebugLogA("[Parameters] defaults.json not found: %ws\n", path.c_str());
		return false;
	}

	json data = json::parse(f, nullptr, false);
	if (data.is_discarded() || !data.is_object())
	{
		DebugLogA("[Parameters] defaults.json is invalid: %ws\n", path.c_str());
		return false;
	}

	auto &toolbar = App::GetInstance()->toolbar;

	for (auto &param : toolbar.parameters)
	{
		if (data.contains(param.name))
		{
			const auto& value = data[param.name];
			if (value.is_number())
			{
				param.SetValue(value.get<float>());
			}
		}
	}
	return true;
}
