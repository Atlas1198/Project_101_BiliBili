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
#include "Behavior.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

enum class GAME_STATE
{
	STATE_BEFORE_COUNTDOWN,
	STATE_COUNTDOWN,
	STATE_PLAY,
	STATE_RESULT,
};

class GameScene;
class BegginningBehavior : public Behavior
{
public:
	BegginningBehavior(GameScene* gameScene) : m_pGameScene(gameScene) {}
	void Update() override;
private:
	GameScene* m_pGameScene = nullptr; // ゲームシーンへのポインタ
};

class CountdownBehavior : public Behavior
{
public:
	CountdownBehavior(GameScene* gameScene) : m_pGameScene(gameScene) {}
	void Update() override;
private:
	GameScene* m_pGameScene = nullptr; // ゲームシーンへのポインタ
};

class PlayBehavior : public Behavior
{
public:
	PlayBehavior(GameScene* gameScene) : m_pGameScene(gameScene) {}
	void Update() override;
private:
	GameScene* m_pGameScene = nullptr; // ゲームシーンへのポインタ
};

class ResultBehavior : public Behavior
{
	enum class SUB_STATE
	{
		CAMERA_MOVE,
		FIRST_WAIT,
		SHOW_RESULT,
		THANK_YOU_SCREEN,
	};

public:
	static constexpr int PRESS_DURATION = 90;
	ResultBehavior(GameScene* gameScene) : m_pGameScene(gameScene) {}
	void Update() override;
	void SetCameraStartPosition(const DirectX::XMFLOAT3& startPosition) { m_cameraStartPosition = startPosition; }
	void SetCameraDestination(const DirectX::XMFLOAT3& destination) { m_cameraDestination = destination; }

private:
	GameScene* m_pGameScene = nullptr;							// ゲームシーンへのポインタ
	SUB_STATE m_subState = SUB_STATE::CAMERA_MOVE;				// リザルトサブ状態
	int m_pressTimer = 0;										// ボタンが押されている時間をカウントするタイマー
	DirectX::XMFLOAT3 m_cameraStartPosition{ 0.0f, 0.0f, 0.0f };// カメラの開始位置(ゲーム終了時に設定)
	DirectX::XMFLOAT3 m_cameraDestination{ 0.0f, 0.0f, 0.0f };	// カメラの目的地(ゲーム終了時に設定)

private:
	void HandleCameraMove();		// カメラ移動状態の処理
	void HandleFirstWait();			// 最初の待機状態の処理
	void HandleShowResult();		// リザルト表示状態の処理
	void HandleThankYouScreen();	// エンディング画面表示状態の処理
};

//ゲームシーンクラス
class GameScene : public SceneBase
{
public:
	static constexpr DirectX::XMFLOAT3 IN_GAME_CAMERA_POSITION = { 0.0f, 70.0f, -50.0f };	//ゲーム中のカメラ位置
	static constexpr DirectX::XMFLOAT3 IN_GAME_CAMERA_TARGET = { 0.0f, 0.0f, 3.5f };	//ゲーム中のカメラターゲット
	static constexpr float IN_GAME_CAMERA_FOV = 20.0f;	//ゲーム中のカメラ視野角
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
	void AddPlayer(uint32_t id, InputManager* pInputManager);
	void SpawnPlayers(InputManager* pInputManager);
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
	BulletManager* m_pBulletManager = nullptr;	//弾管理クラス
	ItemManager* m_pItemManager = nullptr;		//アイテム管理クラス
	BBManager* m_pBBManager = nullptr;			//BB管理クラス
	GameEventManager* m_pGameEventManager = nullptr; //イベント管理クラスのポインタ

	int m_timer = 0; //タイマー

	bool m_isGameOver = false;
	int m_winner = -1;
	int m_character1ID = -1;
	int m_character2ID = -1;
	DirectX::XMFLOAT3 m_lastDamagedPlayerPosition = { 0.0f, 0.0f, 0.0f }; // 最後にダメージを受けたプレイヤーの位置

	Behavior* m_currentBehavior = nullptr; // 現在のビヘイビア
	BegginningBehavior* m_begginningBehavior = nullptr;
	CountdownBehavior* m_countdownBehavior = nullptr;
	PlayBehavior* m_playBehavior = nullptr;
	ResultBehavior* m_resultBehavior = nullptr;

	friend class BegginningBehavior;
	friend class CountdownBehavior;
	friend class PlayBehavior;
	friend class ResultBehavior;

private:
	void ChangeBehavior(GAME_STATE newState);
};