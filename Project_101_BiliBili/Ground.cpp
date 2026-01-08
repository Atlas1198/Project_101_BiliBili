#include "Ground.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
Ground::Ground(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::GROUND, COLLISION_LAYER::GROUND)
{
	m_pColliderSet->AddCollider(
		colliderType,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		scale,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
}

//更新
void Ground::UpdateOverride()
{
}

//衝突解決
void Ground::ResolveCollisionsOverride()
{
}
