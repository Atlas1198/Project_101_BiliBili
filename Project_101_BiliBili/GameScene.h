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
#include "GameEventManager.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

enum class GameState
{
	STATE_BEFORE_COUNTDOWN,
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
		TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
		MeshManager& pMeshManager			//メッシュ管理クラスの参照
	) override;
	void UpdateOverride() override;								//更新
	void ResolveCollisions() override;							//衝突後処理
	void DrawOverride(Renderer& pRenderer) override;			//描画
	void FinalizeOverride() override;							//終了
	void AddPlayer(uint32_t id, InputManager *pInputManager);
	void SpawnPlayers(InputManager *pInputManager);
	void RemovePlayer(uint32_t id);

	void SetGameOver(bool flag, int winner, int character1ID, int character2ID) {
		m_isGameOver = flag;
		m_winner = winner;
		m_character1ID = character1ID;
		m_character2ID = character2ID;
	}

private:
	PlayerManager* m_pPlayerManager = nullptr;	//プレイヤー管理クラス
	FieldManager* m_pFieldManager = nullptr;	//フィールド管理クラス
	GameUIManager* m_pGameUIManager = nullptr;	//ゲームUI管理クラス
	BulletManager *m_pBulletManager = nullptr;	//弾管理クラス
	ItemManager* m_pItemManager = nullptr;		//アイテム管理クラス
	BBManager* m_pBBManager = nullptr;			//BB管理クラス
	GameEventManager* m_pGameEventManager = nullptr; //イベント管理クラスのポインタ

	GameState m_gameState = GameState::STATE_BEFORE_COUNTDOWN; // ゲームの状態

	int m_timer = 0; //タイマー

	bool m_isGameOver = false;
	int m_winner = -1;
	int m_character1ID = -1;
	int m_character2ID = -1;

	bool m_isResultUIShown = false;

private:
	void BeforeCountdownUpdate();	 // カウントダウン前の更新処理
	void CountdownUpdate();			// カウントダウン中の更新処理
	void PlayUpdate();				// プレイ中の更新処理
	void ResultUpdate();			// ゲームオーバー時の更新処理
};
