#pragma once
#include "ObjectBase.h"
#include "InputManager.h"

//プレイヤークラス
class Player : public ObjectBase
{
public:	//公開定数
	inline static float MOVE_SPEED;	//移動速度
	static constexpr float ROTATE_SPEED = 3.0f;	//回転速度
	uint32_t id;								//ID


private:	//非公開メンバ変数
	InputInfo* m_pInputInfo{};	//入力情報構造体
	Player* teammate = nullptr;
	int teamID = -1;

public:	//公開関数
	Player(	//コンストラクタ
		MeshData::MESH_TYPE meshType,			//メッシュタイプ
		DirectX::XMFLOAT3 position,				//座標
		DirectX::XMFLOAT3 rotation,				//回転
		DirectX::XMFLOAT3 scale,				//スケール
		DirectX::XMFLOAT3 velocity,				//移動速度

		uint32_t id,							//ID

		bool isActive = true,					//アクティブフラグ
		ColliderType colliderType =				//コライダータイプ
			ColliderType::BOX,
		DirectX::XMFLOAT3 collisionBoxSize =	//コライダーのボックスサイズ
			DirectX::XMFLOAT3(1.0f, 1.0f,1.0f),
		bool collisionIsTrigger = false			//コライダーのトリガーフラグ
	)
		: ObjectBase(
			meshType,
			position, 
			rotation, 
			scale, 
			velocity, 
			isActive, 
			OBJECT_TAG::PLAYER,
			colliderType, 
			CollisionData::COLLISION_LAYER::PLAYER,
			collisionBoxSize, 
			collisionIsTrigger), 
		id(id) {}
	~Player() {}	//デストラクタ

	//メイン処理関数
	void Initialize(InputManager* pInputManager);	//初期化
	void UpdateOverride() override;					//更新
	void ResolveCollisionsOverride() override;		//衝突解決
	void SetTeamID(int team) { teamID = team; } //チームIDセット
	void BindTeammate(Player* teammate) { this->teammate = teammate; } //味方のセット
	XMFLOAT3 GetPosition() const { return m_position; } //位置取得

private:	//非公開関数
	void Move();	//移動
	void Rotate();	//回転
	void Scale();	//スケール
	void Shoot();	//射撃
};