#pragma once
#include "SceneBase.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "SharedStruct.h"

//前方宣言
class Renderer;			//レンダラー
class InputManager;		//入力管理クラス
class TextureManager;	//テクスチャ管理クラス
class MeshManager;		//メッシュ管理クラス


// シーン管理クラス
class SceneManager
{
public:	//公開定数
	enum SCENE
	{
		SCENE_TITLE,	//タイトルシーン
		SCENE_GAME,		//ゲームシーン
		SCENE_RESULT,	//リザルトシーン
	};

public:
	SceneManager(float windowWidth, float windowHeight);	//コンストラクタ
	~SceneManager();										//デストラクタ

	//メイン処理関数
	void Initialize(
		InputManager* pInputManager,		//入力管理クラスのポインタ
		TextureManager* pTextureManager,	//テクスチャ管理クラスのポインタ
		MeshManager* pMeshManager			//メッシュ管理クラスのポインタ
	);
	void Update();
	void Finalize();

	void SubmitDraws(Renderer& pRenderer);	//描画要求提出

	void ChangeScene(SCENE newScene);	//シーン変更

	void AddPlayer(uint32_t id);
	void SpawnPlayers();
	void RemovePlayer(uint32_t id);

	CameraInfo* GetCameraInfo();	//カメラ情報取得

private:	//メンバ変数
	SceneBase* m_pCurrentScene = nullptr; //現在のシーンクラスのポインタ

	TitleScene* m_pTitleScene = nullptr; //タイトルシーンクラスのポインタ
	GameScene* m_pGameScene = nullptr; //ゲームシーンクラスのポインタ

	InputManager* m_pInputManager = nullptr;		//入力管理クラスのポインタ
	TextureManager* m_pTextureManager = nullptr;	//テクスチャ管理クラスのポインタ
	MeshManager* m_pMeshManager = nullptr;			//メッシュ管理クラスのポインタ
};