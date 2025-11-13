#pragma once
#include "GameObject.h"
#include "InputManager.h"

//プレイヤークラス
class Player : public GameObject
{
public:	//公開定数
	static constexpr float MOVE_SPEED = 0.2f;	//移動速度
	static constexpr float ROTATE_SPEED = 3.0f;	//回転速度

private:	//非公開メンバ変数
	InputInfo* m_pInputInfo{};	//入力情報構造体

public:	//公開関数
	Player(	//コンストラクタ
		DirectX::XMFLOAT3 position,				//座標
		DirectX::XMFLOAT3 rotation,				//回転
		DirectX::XMFLOAT3 scale,				//スケール
		DirectX::XMFLOAT3 velocity,				//移動速度
		bool isActive = true,					//アクティブフラグ
		ColliderType colliderType =				//コライダータイプ
			ColliderType::BOX,
		DirectX::XMFLOAT3 collisionBoxSize =	//コライダーのボックスサイズ
			DirectX::XMFLOAT3(1.0f, 1.0f,1.0f),
		bool collisionIsTrigger = false			//コライダーのトリガーフラグ
	)
		: GameObject(
			position, 
			rotation, 
			scale, 
			velocity, 
			isActive, 
			colliderType, 
			collisionBoxSize, 
			collisionIsTrigger){}
	~Player() {}	//デストラクタ

	//メイン処理関数
	void Initialize(InputManager* pInputManager);	//初期化
	void UpdateOverride() override;					//更新
	void ResolveCollisionsOverride() override;		//衝突解決

private:	//非公開関数
	void Move();	//移動
	void Rotate();	//回転
	void Scale();	//スケール
};