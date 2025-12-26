#pragma once
#include "SceneBase.h"
#include "Camera.h"
#include "PlayerManager.h"
#include "FieldManager.h"
#include "GameUIManager.h"
#include "BulletManager.h"
#include "ItemManager.h"
#include "BBManager.h"
#include "SharedStruct.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

enum class GameState
{
	STATE_COUNTDOWN,
	STATE_PLAY,
	STATE_RESULT
};

//ゲームシーンクラス
class GameScene : public SceneBase
{
public:	//公開関数
	GameScene(float window_width, float window_height);	//コンストラクタ
	~GameScene();										//デストラクタ

	//メイン処理関数
	void InitializeOverride(									//初期化
		InputManager* pInputManager,		//入力マネージャーのポインタ
		TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
		MeshManager& pMeshManager			//メッシュ管理クラスの参照
	) override;
	void UpdateOverride() override;						//更新
	void ResolveCollisions() override;					//衝突後処理
	void DrawOverride(Renderer& pRenderer) override;	//描画
	void FinalizeOverride() override;							//終了
	void AddPlayer(uint32_t id, InputManager *pInputManager);
	void SpawnPlayers(InputManager *pInputManager);
	void RemovePlayer(uint32_t id);

private:
	PlayerManager* m_pPlayerManager = nullptr;	//プレイヤー管理クラス
	FieldManager* m_pFieldManager = nullptr;	//フィールド管理クラス
	GameUIManager* m_pGameUIManager = nullptr;	//ゲームUI管理クラス
	BulletManager *m_pBulletManager = nullptr;	//弾管理クラス
	ItemManager* m_pItemManager = nullptr;		//アイテム管理クラス
	BBManager* m_pBBManager = nullptr;			//BB管理クラス

	GameState m_gameState = GameState::STATE_COUNTDOWN; // ゲームの状態
};
