#pragma once
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//#define STRICT
#include <windows.h>
#include "Engine.h"
#include "Renderer.h"
#include "Camera.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "netcommon.h"
#include "ToolbarControl.h"

//アプリケーションクラス
class App : public olc::net::client_interface<GameMsg>
{
public:
	static constexpr int WINDOW_WIDTH = 1920;	//ウィンドウの幅
	static constexpr int WINDOW_HEIGHT = 1080;	//ウィンドウの高さ
	inline static float DPIScale = 1.0f;	//DPIスケーリング
	HWND hwnd = nullptr;			//ウィンドウハンドル
private:
	HINSTANCE hInstance = nullptr;	//インスタンスハンドル

	WNDCLASSEX wc = {};	//ウィンドウクラス

	App(const App&) = delete;				//コピーコンストラクタ禁止
	void operator=(const App&) = delete;	//代入演算子禁止

private:
	Engine* m_pEngine = nullptr;					//DirectX12エンジンのポインタ
	Renderer* m_pRenderer = nullptr;				//レンダラーのポインタ
	SceneManager* m_pSceneManager = nullptr;		//シーン管理クラスのポインタ
	InputManager* m_pInputManager = nullptr;		//入力管理クラスのポインタ
	TextureManager* m_pTextureManager = nullptr;	//テクスチャ管理クラスのポインタ
	MeshManager* m_pMeshManager = nullptr;			//メッシュ管理クラスのポインタ

public:
	App() {};	//コンストラクタ
	~App(){};	//デストラクタ

	static App* GetInstance();	//シングルトンインスタンス取得

	bool Initialize();	//初期化
	void Run();			//実行
	void Terminate();	//終了
	bool Login();

private:
	void CreateMainWindow(HWND& hwnd, WNDCLASSEX& wc);	//メインウィンドウの生成
	void PrepareInstance();								//インスタンス準備

	void InitInstance();	//インスタンス初期化
	void ReadMessages();
	void WriteMessages();
	void UpdateParameters();
	void Update();			//更新
	void Draw();			//描画

public:
	float moveSpeed = 0.2f;
	PlayerDescription descPlayer;
	std::unordered_map<uint32_t, PlayerDescription> players;
	Vec3 spawnPos = { 0.0f, 0.0f, 0.0f };
	bool isOnline = false;

private:
	int playerCount = 0;
	bool inLobby = true;
	bool waitingForConnection = true;

public:
	ToolbarControl toolbar;
};