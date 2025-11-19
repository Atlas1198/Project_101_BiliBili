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

using json = nlohmann::json;

#pragma comment(lib, "winmm.lib")

int SetSliderRange(HWND hwndTrack, int iMin, int iMax, int iPos);
BOOL SaveTextFile(std::string text, LPCTSTR pszFileName);
std::string CreateParameterString(const ToolbarControl &toolbar);
void InitializeDPIScale(HWND hwnd);
void LoadParameters();

struct ParamUI
{
	std::string name;
	ToolParameter *param;
	HWND hLabel;
	HWND hSlider;
	HWND hValue;
	int min;
	int max;
};

LRESULT CALLBACK ToolDlgProc(HWND hwnd, UINT Message, WPARAM wParam, LPARAM lParam)
{
	App *app = App::GetInstance();

	switch (Message)
	{
	case WM_INITDIALOG:
	{
		// Allocate vector of ParamUI and store on dialog window data
		auto params = new std::vector<ParamUI>();

		// Layout configuration
		const int marginX = 16;
		const int marginY = 8;
		const int labelW = 120;
		//const int sliderW = 240;
		const int sliderW = 360;
		const int valueW = 80;
		//const int controlH = 22;
		const int controlH = 40;
		const int spacingY = 8;
		const int gapV = 100;
		int safeSpaceRight = 50;

		// Prepare the parameters to display.
		// Add entries here for each parameter you want to expose in the dialog.
		// Example entry: Move Speed
		params->push_back(
			ParamUI{
				"移動スピード",
				&app->toolbar.parameters[0],
				NULL,
				NULL,
				NULL,
				app->toolbar.parameters[0].min,
				app->toolbar.parameters[0].max
			}
		);

		int buttonWidth = 200;
		int buttonHeight = 50;

		SetWindowPos(hwnd, NULL, 0, 0,
			(marginX*2 + labelW + sliderW + valueW + safeSpaceRight),
			(marginY*2 + (controlH + spacingY) * params->size() + gapV + buttonHeight),
			SWP_NOZORDER);

		// Create actual controls
		HINSTANCE hInst = GetModuleHandle(NULL);

		for (size_t i = 0; i < params->size(); ++i)
		{
			int y = marginY + static_cast<int>(i) * (controlH + spacingY);

			// Label
			params->at(i).hLabel = CreateWindowEx(
				0, "STATIC", params->at(i).name.c_str(),
				WS_CHILD | WS_VISIBLE | SS_CENTERIMAGE,
				marginX, y, labelW, controlH,
				hwnd, (HMENU)(INT_PTR)(1000 + (int)i * 3 + 0), hInst, NULL);

			// Slider (trackbar)
			params->at(i).hSlider = CreateWindowEx(
				0, TRACKBAR_CLASS, NULL,
				WS_CHILD | WS_VISIBLE | TBS_AUTOTICKS | TBS_ENABLESELRANGE,
				marginX + labelW + 8, y, sliderW, controlH,
				hwnd, (HMENU)(INT_PTR)(1000 + (int)i * 3 + 1), hInst, NULL);

			// Value static
			char buf[64];
			snprintf(buf, sizeof(buf), "%f", params->at(i).param->GetValue());
			params->at(i).hValue = CreateWindowEx(
				0, "STATIC", buf,
				WS_CHILD | WS_VISIBLE | SS_RIGHT | SS_CENTERIMAGE,
				marginX + labelW + 8 + sliderW + 8, y, valueW, controlH,
				hwnd, (HMENU)(INT_PTR)(1000 + (int)i * 3 + 2), hInst, NULL);

			// Configure slider range and initial position
			SetSliderRange(
				params->at(i).hSlider,
				params->at(i).min,
				params->at(i).max,
				params->at(i).param->GetIntValue()
			);
		}

		CreateWindowEx(0, "BUTTON", "ファイル保存",
			WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
			marginX + labelW + sliderW + valueW + safeSpaceRight - buttonWidth, marginY + static_cast<int>(params->size()) * (controlH + spacingY) + gapV, buttonWidth, buttonHeight,
			hwnd, (HMENU)IDC_BUTTON1, hInst, NULL);

		// Save params vector pointer on window for later use
		SetWindowLongPtr(hwnd, GWLP_USERDATA, (LONG_PTR)params);

		SetWindowPos(hwnd, NULL, 0, 0,
			(marginX * 2 + labelW + sliderW + valueW + safeSpaceRight),
			(marginY * 2 + (controlH + spacingY) * params->size() + gapV + buttonHeight),
			SWP_NOZORDER);

		return TRUE;
	}
	case WM_HSCROLL:
	{
		HWND hwndScrollBar = (HWND)lParam;
		// If lParam is NULL the message may come from keyboard; try to handle only real slider messages
		if (hwndScrollBar == NULL)
			break;

		auto params = reinterpret_cast<std::vector<ParamUI>*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (params)
		{
			for (auto &p : *params)
			{
				if (p.hSlider == hwndScrollBar)
				{
					int iPos = (int)SendMessage(hwndScrollBar, TBM_GETPOS, 0, 0);
					// Update parameter
					p.param->SetValue(iPos);

					// Update value text
					std::string s = std::to_string(p.param->GetValue());
					SetWindowText(p.hValue, s.c_str());

					// If the slider that came from the resource (IDC_SLIDER1) was used elsewhere in code,
					// keep compatibility by updating the original control text as well:
					// (optional - uncomment if needed)
					// if (GetDlgItem(app->toolbar.hToolbar, IDC_SLIDER1) == hwndScrollBar) {
					//     SetDlgItemText(app->toolbar.hToolbar, IDC_STATIC1, std::to_string(app->toolbar.speed.GetValue()).c_str());
					// }

					if (LOWORD(wParam) == SB_THUMBPOSITION) {
						SetFocus(App::GetInstance()->hwnd);
					}
					break;
				}
			}
		}
		break;
	}
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDC_BUTTON1)
		{
			OPENFILENAME ofn;
			char szFileName[MAX_PATH] = "defaults";

			ZeroMemory(&ofn, sizeof(ofn));

			ofn.lStructSize = sizeof(ofn); // SEE NOTE BELOW
			ofn.hwndOwner = hwnd;
			//ofn.lpstrFilter = "Text Files (*.txt)\0*.txt\0All Files (*.*)\0*.*\0";
			ofn.lpstrFilter = "JSON Files (*.json)\0";
			ofn.lpstrFile = szFileName;
			ofn.nMaxFile = MAX_PATH;
			ofn.Flags = OFN_EXPLORER | OFN_HIDEREADONLY;
			ofn.lpstrDefExt = "json";

			if (GetOpenFileName(&ofn))
			{
				nlohmann::json j;

				for (ToolParameter &param : app->toolbar.parameters)
				{
					j[param.name] = param.GetValue();
				}

				SaveTextFile(j.dump(4), szFileName);
			}
		}
		break;
	}
	case WM_LBUTTONDOWN: {
		app->toolbar.mousedown = true;
		SetCapture(hwnd);
		GetCursorPos(&app->toolbar.lastLocation);
		RECT rect;
		GetWindowRect(hwnd, &rect);
		app->toolbar.lastLocation.x = app->toolbar.lastLocation.x - rect.left;
		app->toolbar.lastLocation.y = app->toolbar.lastLocation.y - rect.top;
		break;
	}
	case WM_LBUTTONUP: {
		app->toolbar.mousedown = false;
		ReleaseCapture();
		break;
	}
	case WM_MOUSEMOVE: {
		if (app->toolbar.mousedown) {
			POINT currentpos;
			GetCursorPos(&currentpos);
			RECT rect;
			GetWindowRect(hwnd, &rect);
			int x = currentpos.x - app->toolbar.lastLocation.x;
			int y = currentpos.y - app->toolbar.lastLocation.y;
			MoveWindow(hwnd, x, y, rect.right - rect.left, rect.bottom - rect.top, false);
		}
		break;
	}
	case WM_DESTROY:
	{
		// Clean up allocated params vector
		auto params = reinterpret_cast<std::vector<ParamUI>*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
		if (params)
		{
			delete params;
			SetWindowLongPtr(hwnd, GWLP_USERDATA, 0);
		}
		return FALSE;
	}
	default:
		return FALSE;
	}
	return TRUE;
}

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

		App::GetInstance()->toolbar.hToolbar = CreateDialog(
			GetModuleHandle(NULL),
			MAKEINTRESOURCE(IDD_DIALOG1),
			hwnd,               // owner = main window
			ToolDlgProc);

		// Ensure the dialog is not "always on top" of all windows.
		if (App::GetInstance()->toolbar.hToolbar)
		{
			// Make sure it's owned by main window (redundant if hwnd passed above, but harmless)
			SetWindowLongPtr(App::GetInstance()->toolbar.hToolbar, GWLP_HWNDPARENT, (LONG_PTR)hwnd);

			// Remove TOPMOST if it was set by resource or elsewhere
			SetWindowPos(
				App::GetInstance()->toolbar.hToolbar,
				HWND_NOTOPMOST,
				0, 0, 0, 0,
				SWP_NOMOVE | SWP_NOSIZE);
		}
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
	LoadParameters();

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


	UpdateParameters();

	if (msgboxID == IDYES)
	{
		isOnline = true;
		if (!Login()) return;
	}
	else
	{
		isOnline = false;

		for (int i = 0; i < 4; i++)
		{
			PlayerDescription newDesc;
			newDesc.uniqueID = static_cast<uint32_t>(i);
			newDesc.ingameID = static_cast<uint32_t>(i);
			newDesc.pos = spawnPos;
			players.emplace(newDesc.uniqueID, newDesc);

			m_pSceneManager->AddPlayer(newDesc.uniqueID);
		}

		if (toolbar.hToolbar != NULL)
		{
			ShowWindow(toolbar.hToolbar, SW_SHOW);
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
	m_pInputManager->Update();			//入力管理クラスの更新
	m_pSceneManager->Update();			//ゲームの更新
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

			/*
			case(GameMsg::Server_RespondDesignerRequest):
			{
				bool result;
				msg >> result;

				if (result)
				{
					isDesigner = true;
				}
				else
				{
					waitingToOpenTool = false;
				}
				break;
			}

			case(GameMsg::Server_ChangeParameter):
			{
				msg >> moveSpeed;
			}
			*/
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

		/*
		if (requestingDesignerRights)
		{
			olc::net::message<GameMsg> msg;
			msg.header.id = GameMsg::Client_RequestDesigner;
			Send(msg);
			requestingDesignerRights = false;
		}

		if (wnd.submit)
		{
			olc::net::message<GameMsg> msg;
			msg.header.id = GameMsg::Client_RequestChangeParameter;
			msg << wnd.speedField;
			Send(msg);
			wnd.submit = false;
		}
		*/
	}
}

void App::UpdateParameters()
{
	Player::MOVE_SPEED = toolbar.parameters[0].GetValue();
}

int SetSliderRange(HWND hwndTrack, int iMin, int iMax, int iPos) {
	SendMessage(hwndTrack, TBM_SETRANGE,
		(WPARAM)TRUE, // redraw flag
		(LPARAM)MAKELONG(iMin, iMax)); // min. & max. positions
	SendMessage(hwndTrack, TBM_SETSEL,
		(WPARAM)FALSE, // redraw flag
		(LPARAM)MAKELONG(iMin, iMax));
	SendMessage(hwndTrack, TBM_SETPAGESIZE,
		0, (LPARAM)(iMax - iMin) / 10); // new page size
		SendMessage(hwndTrack, TBM_SETPOS,
			(WPARAM)TRUE, // redraw flag
			(LPARAM)iPos);

	SetFocus(hwndTrack);
	return 1;
}

BOOL SaveTextFile(std::string text, LPCTSTR pszFileName)
{
	HANDLE hFile;
	BOOL bSuccess = FALSE;

	hFile = CreateFile(pszFileName, GENERIC_WRITE, 0, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		DWORD dwWritten;

		if (WriteFile(hFile, text.c_str(), text.size(), &dwWritten, NULL))
			bSuccess = TRUE;
		CloseHandle(hFile);
	}
	return bSuccess;
}

std::string CreateParameterString(const ToolbarControl& toolbar)
{
	std::string paramStr = "";

	auto params = reinterpret_cast<std::vector<ParamUI>*>(GetWindowLongPtr(toolbar.hToolbar, GWLP_USERDATA));
	if (params)
	{
		for (auto &p : *params)
		{
			paramStr += p.name + ": " + std::to_string(p.param->GetValue()) + "\n";
		}
	}
	return paramStr;
}

void InitializeDPIScale(HWND hwnd)
{
	float dpi = GetDpiForWindow(hwnd);
	App::DPIScale = dpi / USER_DEFAULT_SCREEN_DPI;
}

void LoadParameters()
{
	std::ifstream f("asset/defaults.json");
	json data = json::parse(f);

	auto &toolbar = App::GetInstance()->toolbar;

	for (auto &param : toolbar.parameters)
	{
		if (data.contains(param.name))
		{
			param.SetValue(data[param.name].get<float>() * param.divisionBy);
		}
	}
}