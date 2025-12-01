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

using json = nlohmann::json;

#pragma comment(lib, "winmm.lib")

void InitializeDPIScale(HWND hwnd);
void LoadParametersJSON();

DatabaseManager* dbManager = DatabaseManager::GetInstance();

//ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:

		InitializeDPIScale(hwnd);

		SetWindowPos(hwnd, NULL, 0, 0, 
			App::WINDOW_WIDTH * App::DPIScale, 
			App::WINDOW_HEIGHT * App::DPIScale,
			SWP_NOZORDER);

		break;
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

	case DatabaseManager::WM_FIREBASE_UPDATE:
		dbManager->ApplyFirebaseStreamData();
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
	CreateMainWindow(hwnd, wc);	//メインウィンドウの生成

	PrepareInstance(); // インスタンス準備

	InitInstance(); // インスタンス初期化

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

	int msgboxID = MessageBox(
		NULL,
		"オンラインモードに入りますか？",
		"モード選択",
		MB_ICONQUESTION | MB_YESNO
	);

	if (msgboxID == IDYES)
	{
		LoadParametersJSON();
		UpdateParameters();
		isOnline = true;
		if (!Login()) return;
	}
	else
	{
		isOnline = false;

		m_pSceneManager->SpawnPlayers();

		int msgboxID = MessageBox(
			NULL,
			"ビリビリアプリ使いますか？",
			"パラメーター調整",
			MB_ICONQUESTION | MB_YESNO
		);

		if (msgboxID == IDYES)
		{
			// Start background streaming thread (will PostMessage to main window on updates)
			curl_global_init(CURL_GLOBAL_DEFAULT);
			dbManager->StartFirebaseStream();
		}
		else
		{
			LoadParametersJSON();
			UpdateParameters();
		}
	}

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
				wsprintf(debugStr, "DX21 プロジェクト ");	//ウィンドウタイトル文字列
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
				m_pInputManager->keyCopy(); //入力管理クラスのキー情報コピー
			}
		}
	} while (msg.message != WM_QUIT);	//終了メッセージが来るまで
}

//終了
void App::Terminate()
{
	// Stop the Firebase streaming thread before tearing down other systems
	dbManager->StopFirebaseStream();

	m_pEngine->Terminate(); //DirectX12エンジンの終了

	//delete m_pCamera;		//カメラの解放
	delete m_pInputManager;	//入力管理クラスの解放
	delete m_pRenderer;		//レンダラーの解放
	delete m_pSceneManager;	//シーン管理の解放

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

	RECT wrc = { 0,0, WINDOW_WIDTH, WINDOW_HEIGHT };//ウィンドウサイズを決める

	//ウィンドウサイズを補正
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//ウィンドウオブジェクトの生成
	hwnd = CreateWindowEx(
		0,						// 拡張スタイル
		wc.lpszClassName,		//クラス名の指定
		_T("DX12_Application"),	//タイトルバーの文字
		WS_OVERLAPPEDWINDOW,	//ウィンドウスタイル
		CW_USEDEFAULT,			//表示X座標はOSにお任せします
		CW_USEDEFAULT,			//表示Y座標はOSにお任せします
		wrc.right - wrc.left,	//ウィンドウ横幅
		wrc.bottom - wrc.top,	//ウィンドウ縦幅
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
}

//インスタンス初期化
void App::InitInstance()
{

	//DirectX12エンジン初期化
	m_pEngine->Initialize(
		hwnd,			//ウィンドウハンドル
		WINDOW_WIDTH,	//フレームバッファの幅
		WINDOW_HEIGHT	//フレームバッファの高さ
	);

	//デバイスの取得
	auto pDevice = m_pEngine->GetDevice();

	//レンダラー初期化
	m_pRenderer->Initialize(
		pDevice,							//デバイス
		m_pSceneManager->GetCameraInfo()	//カメラ情報構造体
	);

	//テクスチャ管理クラス初期化
	m_pTextureManager->Initialize(
		pDevice,	//デバイス
		512			//最大ディスクリプタ数
	);

	//メッシュ管理クラス初期化
	m_pMeshManager->Initialize(
		pDevice	//デバイス
	);

	//レンダーを開始してコマンドリストをオープン
	m_pEngine->RenderBegin();	

	//シーン管理クラス初期化
	m_pSceneManager->Initialize(
		m_pInputManager,	//入力管理クラス
		m_pTextureManager,	//テクスチャ管理クラス
		m_pMeshManager		//メッシュ管理クラス
	);

	//入力管理クラス初期化
	m_pInputManager->Initialize();

	//レンダーを終了してコマンドリストをクローズ
	m_pEngine->RenderEnd();
}

//更新
void App::Update()
{
	if (isOnline && waitingForConnection) return;
	//現在のバックバッファインデックスを取得
	const UINT backIdx = m_pEngine->GetCurrentBufferIndex();

	//各種更新処理
	m_pRenderer->BeginFrame(backIdx);	//フレーム開始（内部キューをクリア）

	if (!EventManager::GetInstance()->gameOver)
	{
		m_pInputManager->Update();			//入力管理クラスの更新
		m_pSceneManager->Update();			//ゲームの更新
	}

	m_pRenderer->Update(backIdx, *m_pSceneManager->GetCameraInfo());		//レンダラーの更新
}

//描画
void App::Draw()
{
	//描画開始
	m_pEngine->RenderBegin();

	//保留中のテクスチャをアップロード
	m_pTextureManager->UploadPendingTextures(m_pEngine->GetCommandList());

	//ゲームシーンの描画要求をシーンに提出
	m_pSceneManager->SubmitDraws(*m_pRenderer);

	//シーンの描画
	m_pRenderer->Draw(
		m_pEngine->GetCurrentBufferIndex(),	//バッファインデックス
		m_pEngine->GetCommandList(),		//コマンドリスト
		*m_pTextureManager					//テクスチャ管理クラス
	);

	//描画終了
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
					std::cout << "Assigned Client ID = " << descPlayer.uniqueID << "\n";
					break;
				}

				case(GameMsg::Game_AddPlayer):
				{
					PlayerDescription newDesc;

					msg >> newDesc >> playerCount;

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
					players.erase(nRemovalID);
					m_pSceneManager->RemovePlayer(nRemovalID);
					break;
				}

				case(GameMsg::Game_UpdatePlayer):
				{
					PlayerDescription desc;
					msg >> desc;
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
	float dpi = GetDpiForWindow(hwnd);
	App::DPIScale = dpi / USER_DEFAULT_SCREEN_DPI;
}

void LoadParametersJSON()
{
	std::ifstream f("asset/defaults.json");
	json data = json::parse(f);

	auto &toolbar = App::GetInstance()->toolbar;

	for (auto &param : toolbar.parameters)
	{
		if (data.contains(param.name))
		{
			param.SetValue(data[param.name].get<float>());
		}
	}
}