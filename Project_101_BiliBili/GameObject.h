#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include "Collider.h"
#include "SharedStruct.h"

//ゲームオブジェクトクラス
//全ゲームオブジェクトの基底クラス
class GameObject
{
public:	//公開関数
	GameObject(	//コンストラクタ
		DirectX::XMFLOAT3 position,				//座標
		DirectX::XMFLOAT3 rotation,				//回転
		DirectX::XMFLOAT3 scale,				//スケール
		DirectX::XMFLOAT3 velocity,				//移動速度
		bool isActive = true,					//アクティブフラグ
		ColliderType colliderType =				//コライダータイプ
			ColliderType::BOX,	
		DirectX::XMFLOAT3 collisionBoxSize =	//コライダーのボックスサイズ
			DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		bool collisionIsTrigger = false			//コライダーのトリガーフラグ
	);
	~GameObject();	//デストラクタ

	void Update();
	virtual void UpdateOverride() = 0;

	//衝突判定関連
	void ResolveCollisions();							//衝突解決
	virtual void ResolveCollisionsOverride() = 0;		//衝突解決(固有処理用、派生クラスでオーバーライド)
	void AddCollisionInfo(const CollisionInfo& info);	//衝突情報の追加
	void ClearCollisionInfos();							//衝突情報のクリア

	//ゲッター
	const DirectX::XMMATRIX GetWorldMatrix() const;					//ワールド行列の取得
	const DirectX::XMFLOAT3 GetPosition() const;					//位置の取得
	const DirectX::XMFLOAT3 GetRotation() const;					//回転の取得
	const DirectX::XMFLOAT3 GetScale() const;						//スケールの取得
	const DirectX::XMFLOAT4 GetColor() const;						//色RGBAの取得
	const bool IsActive() const;									//アクティブかどうかを取得
	const bool IsDrawn() const;										//描画フラグの取得
	Collider* GetCollider() const;									//コライダーの取得
	const std::vector<CollisionInfo>* GetCollisionInfos() const;	//衝突情報配列の取得

	//セッター
	void SetPosition(DirectX::XMFLOAT3 position);	//位置の設定
	void SetRotation(DirectX::XMFLOAT3 rotation);	//回転の設定
	void SetScale(DirectX::XMFLOAT3 scale);			//スケールの設定
	void SetColor(DirectX::XMFLOAT4 color);			//色RGBAの設定
	void SetActive(bool isActive);					//アクティブフラグの設定

protected:	//非公開メンバ変数
	DirectX::XMFLOAT3 m_position{};						//位置
	DirectX::XMFLOAT3 m_rotation{};						//回転
	DirectX::XMFLOAT3 m_scale{ 1.0f,1.0f,1.0f };		//スケール
	DirectX::XMFLOAT4 m_color{ 1.0f,1.0f,1.0f,1.0f };	//色RGBA

	DirectX::XMFLOAT3 m_velocity{};	//移動速度
	bool m_isActive = false;		//アクティブフラグ
	bool m_isDrawn = true;			//描画フラグ

	//コライダー関連
	Collider* m_pCollider = nullptr;				//コライダー
	std::vector<CollisionInfo> m_collisionInfos;	//衝突情報の配列
};