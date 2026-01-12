#include "LineBB.h"
#include <DirectXMath.h>
#include <algorithm>
#include "CollisionManager.h"

using namespace DirectX;
using namespace CollisionData;

LineBB::LineBB(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	:  ObjectBase(
		meshType,
		position,
		rotation,
		scale,
		velocity,
		isActive,
		OBJECT_TAG::BB_LINE,
		COLLISION_LAYER::BB_LINE
		)
{
	m_raycastSegment.layer = COLLISION_LAYER::BB_LINE;
	m_raycastSegment.layerMask = GetLayerMask(COLLISION_LAYER::BB_LINE);

	m_pColliderSet->AddCollider(
		colliderType,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		collisionBoxSize,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
}

//更新
void LineBB::UpdateOverride()
{
	//ラインの設定
	SetLine(
		m_edgePos[0],
		m_edgePos[1]
	);

	//壁との衝突収集
	CheckRaycastCollision();

	//衝突点取得
	GetClosestWallCollisionPoints();
}

//衝突解決
void LineBB::ResolveCollisionsOverride()
{
}

//エッジの位置設定
void LineBB::SetEdgePos(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	m_edgePos[0] = start;
	m_edgePos[1] = end;
}

//壁との衝突点取得
DirectX::XMFLOAT3 LineBB::GetWallCollisionPoint() const
{
	return m_wallCollisionPoint;
}

//レイキャストセグメント取得
CollisionData::RaycastSegment& LineBB::GetRaycastSegment()
{
	return m_raycastSegment;
}

//壁との衝突点収集
void LineBB::GetClosestWallCollisionPoints()
{
	for(auto & hitInfo : m_raycastSegment.hitInfos)
	{
		if (hitInfo.opponent->GetOwnerTag() == OBJECT_TAG::WALL)
		{
			m_wallCollisionPoint = hitInfo.hitPoint;
			return;	//近い順にソート済みなので最初の1個だけ取得して終了
		}
	}
}

//衝突マネージャーの設定
void LineBB::SetCollisionManager(CollisionManager* pCollisionManager)
{
	m_pCollisionManager = pCollisionManager;
}

//ラインの設定
void LineBB::SetLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	XMFLOAT3 dir{};	//方向ベクトル
	dir = 
	{
		end.x - start.x,
		end.y - start.y,
		end.z - start.z
	};

	const float length = sqrtf(
		dir.x * dir.x +
		dir.y * dir.y +
		dir.z * dir.z
	);
	m_length = length;

	if (length < 1e-6f)
	{
		m_position = start;
		m_scale.y = 0.0f;
		m_rotation = { 0.0f, 0.0f, 0.0f };
		return;
	}

	m_position = 
	{
		(start.x + end.x) * 0.5f,
		(start.y + end.y) * 0.5f,
		(start.z + end.z) * 0.5f
	};

	m_scale.y = length;

	const float invLength = 1.0f / length;
	const float dx = dir.x * invLength;
	const float dy = dir.y * invLength;
	const float dz = dir.z * invLength;

	const float dyClamped = std::clamp(dy, -1.0f, 1.0f);

	float yaw = 0.0f;
	const float xzLenSq = dx * dx + dz * dz;
	if (xzLenSq > 1e-8f)
	{
		yaw = atan2f(dx, dz);
	}

	const float pitch = acosf(dyClamped);
	const float roll = 0.0f;
	m_rotation = 
	{
		XMConvertToDegrees(pitch),
		XMConvertToDegrees(yaw),
		XMConvertToDegrees(roll)
	};

	////座標
	//XMFLOAT3 midPos{};	//中点座標
	//midPos.x = (start.x + end.x) * 0.5f;
	//midPos.y = (start.y + end.y) * 0.5f;
	//midPos.z = (start.z + end.z) * 0.5f;
	//m_position = midPos;	//位置設定

	////長さ
	//m_length = sqrtf(
	//	(start.x - end.x) * (start.x - end.x) +
	//	(start.y - end.y) * (start.y - end.y) +
	//	(start.z - end.z) * (start.z - end.z)
	//);
	//m_scale.y = m_length;	//スケール設定	

	//XMFLOAT3 startPosLocal{};	//ローカル座標系での開始位置
	//XMFLOAT3 endPosLocal{};		//ローカル座標系での終了位置
	//startPosLocal.x = start.x - midPos.x;
	//startPosLocal.y = start.y - midPos.y;
	//startPosLocal.z = start.z - midPos.z;
	//endPosLocal.x = end.x - midPos.x;
	//endPosLocal.y = end.y - midPos.y;
	//endPosLocal.z = end.z - midPos.z;

	////Y軸回転
	//float angleY = atan2f(endPosLocal.x - startPosLocal.x, endPosLocal.z - startPosLocal.z);
	//m_rotation.y = XMConvertToDegrees(angleY);
}

//壁と最も近い衝突点の収集
void LineBB::CheckRaycastCollision()
{
	m_raycastSegment.startPoint = m_edgePos[0];
	m_raycastSegment.endPoint = m_edgePos[1];
	m_pCollisionManager->RaycastSegmentQuery(m_raycastSegment);
}
