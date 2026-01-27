#pragma once
#include "ObjectBase.h"
#include "InputManager.h"
#include "BulletManager.h"
#include "SharedStruct.h"
#include "FrameTimer.h"

//プレイヤークラス
class Player : public ObjectBase
{
public:	//公開定数
	inline static float MOVE_SPEED = 0.2f;		//移動速度
	inline static float BULLET_SPEED = 0.2f;
	static constexpr float ROTATE_SPEED = 3.0f;	//回転速度
	static constexpr float GRAVITY = 0.02f;		//重力
	static constexpr float RUN_DELAY = 0.5f;
	static constexpr float RUN_MODIFIER = 0.2f;
	static constexpr float BB_SLOW_MOVE_MODIFIER = 0.8f;
	uint32_t id;								//ID


private:	//非公開メンバ変数
	InputInfo* m_pInputInfo{};	//入力情報構造体
	PlayerInfo info{};			//プレイヤー情報構造体
	Player* teammate = nullptr;
	int teamID = -1;
	int characterID = -1;
	BulletManager *m_pBulletManager = nullptr;
	bool m_isGrounded = false;
	bool m_isSpringJump = false;
	int m_ignoreCollisionFrame = 3;
	int direction = 0; // 移動方向
	int minAnimIndex = 0; // アニメーションの最小インデックス
	int maxAnimIndex = 0; // アニメーションの最大インデックス
	int animUpdateRate = 10; // アニメーションの更新速度
	bool isMoving = false; // 移動中フラグ
	bool isShooting = false; // 射撃中フラグ
	int shootAnimDuration = 5; // 射撃アニメーションの持続フレーム数
	bool bbActive = false; // BBアクティブフラグ
	bool canRun = false;
	bool runTimerStarted = false;
	bool bbSlowMoveSpeed = false;
	FrameTimer runTimer; // 走行タイマー
	FrameTimer gameTimer;

public:	//公開関数
	Player(	//コンストラクタ
		MESH_TYPE meshType,			//メッシュタイプ
		DirectX::XMFLOAT3 position,				//座標
		DirectX::XMFLOAT3 rotation,				//回転
		DirectX::XMFLOAT3 scale,				//スケール
		DirectX::XMFLOAT3 velocity,				//移動速度

		uint32_t id,							//ID

		bool isActive = true,					//アクティブフラグ
		ColliderType colliderType =				//コライダータイプ
		ColliderType::BOX,
		DirectX::XMFLOAT3 collisionBoxSize =	//コライダーのボックスサイズ
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		bool collisionIsTrigger = false			//コライダーのトリガーフラグ
	);

	~Player() {}	//デストラクタ

	//メイン処理関数
	void Initialize(InputManager* pInputManager, BulletManager* pBulletManager);	//初期化
	void UpdateOverride() override;					//更新
	void ResolveCollisionsOverride() override;		//衝突解決
	void SetTeamID(int team) { teamID = team; } //チームIDセット
	void BindTeammate(Player* teammate) { this->teammate = teammate; } //味方のセット
	int GetTeamID() const { return teamID; } //チームID取得

	void SetCharacterID(int character) { characterID = character; } //キャラクターIDセット
	void SetPlayerInfo(const PlayerInfo& info) { this->info = info; }	//プレイヤー情報構造体セット

	void Reset();

	int GetCharacterID() const { return characterID; } //キャラクターID取得
	PlayerInfo GetPlayerInfo() const { return info; }					//プレイヤー情報構造体取得
	void SetBB(bool isActive); // BBセット

private:	//非公開関数
	void Move();	//移動
	void Rotate();	//回転
	void Scale();	//スケール
	void Shoot();	//射撃
	void UpdateAnimation(); //アニメーション更新
};