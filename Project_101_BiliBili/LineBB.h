#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "ObjectBase.h"
#include "SharedStruct.h"

//前方宣言
class CollisionManager;

//BBのラインクラス
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
		);
	~LineBB() {}	//デストラクタ
	//メイン処理関数
	void UpdateOverride() override;					//更新
	void ResolveCollisionsOverride() override;		//衝突解決

	void SetEdgePos(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);	//エッジの位置設定

	CollisionData::RaycastSegment& GetRaycastSegment();	//レイキャストセグメント取得
	void GetClosestWallCollisionPoints();					//壁との衝突点収集

	void SetCollisionManager(CollisionManager* pCollisionManager);	//衝突マネージャーの設定

	DirectX::XMFLOAT3 GetWallCollisionPoint() const;	//壁と最も近い衝突点の取得

private:
	float m_length = 0.0f;						//ラインの長さ
	DirectX::XMFLOAT3 m_edgePos[2]{};			//エッジの位置(始点、終点)
	DirectX::XMFLOAT3 m_wallCollisionPoint{};	//壁との衝突点(最も近い点)

	CollisionData::RaycastSegment m_raycastSegment{};	//レイキャストセグメント

	CollisionManager* m_pCollisionManager = nullptr; //衝突マネージャーへのポインタ

private:
	void SetLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end);	//ラインの設定

	void CheckRaycastCollision(); //壁との衝突収集
};