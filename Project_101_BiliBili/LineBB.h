#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "ObjectBase.h"
#include "SharedStruct.h"

class LineBB : public ObjectBase
{
	public:
	LineBB(	//コンストラクタ
		MeshData::MESH_TYPE meshType,			//メッシュタイプ
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
	)
		: ObjectBase(
			meshType,
			position,
			rotation,
			scale,
			velocity,
			isActive,
			OBJECT_TAG::BB_LINE,
			colliderType,
			CollisionData::COLLISION_LAYER::BB_LINE,
			collisionBoxSize,
			collisionIsTrigger)
	{
	}
	~LineBB() {}	//デストラクタ
	//メイン処理関数
	void UpdateOverride() override;					//更新
	void ResolveCollisionsOverride() override;		//衝突解決

	void SetEdgePos(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);	//エッジの位置設定

	std::vector<DirectX::XMFLOAT3> GetWallCollisionPoints() const;	//壁との衝突点取得

private:
	float m_length = 0.0f;
	DirectX::XMFLOAT3 m_edgePos[2]{};
	std::vector<DirectX::XMFLOAT3> m_wallCollisionPoints{};

private:
	void SetLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);	//ラインの設定

	void CollectWallCollisions(); //壁との衝突収集
};