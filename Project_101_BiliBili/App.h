#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#include "Engine.h"
#include "Renderer.h"
#include "Camera.h"
#include "SceneManager.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

//アプリケーションクラス
class App
{
public:
	static constexpr int WINDOW_WIDTH = 1280;	//ウィンドウの幅
	static constexpr int WINDOW_HEIGHT = 720;	//ウィンドウの高さ
private:
	HINSTANCE hInstance = nullptr;	//インスタンスハンドル
	HWND hwnd = nullptr;			//ウィンドウハンドル

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

private:
	void CreateMainWindow(HWND& hwnd, WNDCLASSEX& wc);	//メインウィンドウの生成
	void PrepareInstance();								//インスタンス準備

	void InitInstance();	//インスタンス初期化
	void Update();			//更新
	void Draw();			//描画
};