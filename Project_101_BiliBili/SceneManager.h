#pragma once
#include "SceneBase.h"
#include "GameScene.h"
#include "TitleScene.h"
#include "ControllerScene.h"
#include "CharacterScene.h"
#include "SharedStruct.h"

//前方宣言
class Renderer;			//レンダラー
class InputManager;		//入力管理クラス
class TextureManager;	//テクスチャ管理クラス
class MeshManager;		//メッシュ管理クラス

enum class SCENE_TYPE
{
	SCENE_NONE = 0,		//シーン無し
	SCENE_TITLE,		//タイトルシーン
	SCENE_CONTROLLER,	//コントローラー設定シーン
	SCENE_CHARACTER,	//キャラクター選択シーン
	SCENE_GAME,			//ゲームシーン
	SCENE_RESULT,		//リザルトシーン
};

// シーン管理クラス
class SceneManager
{
public:	//公開定数

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

	void ReserveChangeScene(SCENE_TYPE newScene);	//シーン変更予約
	void ChangeScene(SCENE_TYPE newScene);			//シーン変更

	void AddPlayer(uint32_t id);
	void SpawnPlayers();
	void RemovePlayer(uint32_t id);

	CameraInfo* GetCameraInfo();	//カメラ情報取得

private:	//メンバ変数
	SCENE_TYPE m_currentScene = SCENE_TYPE::SCENE_NONE; //現在のシーン
	SceneBase* m_pCurrentScene = nullptr; //現在のシーンクラスのポインタ

	TitleScene* m_pTitleScene = nullptr;			//タイトルシーンクラスのポインタ
	ControllerScene* m_pControllerScene = nullptr;	//コントローラーシーンクラスのポインタ
	CharacterScene* m_pCharacterScene = nullptr;	//キャラクターシーンクラスのポインタ
	GameScene* m_pGameScene = nullptr;				//ゲームシーンクラスのポインタ

	InputManager* m_pInputManager = nullptr;		//入力管理クラスのポインタ
	TextureManager* m_pTextureManager = nullptr;	//テクスチャ管理クラスのポインタ
	MeshManager* m_pMeshManager = nullptr;			//メッシュ管理クラスのポインタ

	bool m_sceneChangeReserved = false;	//シーン変更予約フラグ
	SCENE_TYPE m_reservedScene = SCENE_TYPE::SCENE_NONE;	//予約されたシーン
};