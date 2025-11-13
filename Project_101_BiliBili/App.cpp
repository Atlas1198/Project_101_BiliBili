#include <windows.h>
#include <algorithm>
#include <mmsystem.h>
#include "App.h"
#include <tchar.h>
#include "Keyboard.h"

#pragma comment(lib, "winmm.lib")

// RAY TRACING

//ウィンドウプロシージャ
LRESULT WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
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
				//更新処理
				Update();	//更新

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
	//現在のバックバッファインデックスを取得
	const UINT backIdx = m_pEngine->GetCurrentBufferIndex();

	//各種更新処理
	m_pRenderer->BeginFrame(backIdx);	//フレーム開始（内部キューをクリア）
	m_pInputManager->Update();			//入力管理クラスの更新
	m_pSceneManager->Update();			//ゲームの更新
	m_pRenderer->Update(backIdx);		//レンダラーの更新
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
