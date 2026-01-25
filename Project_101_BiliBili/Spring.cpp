#include "Spring.h"
#include "EventManager.h"

using namespace DirectX;
using namespace CollisionData;

// コンストラクタ
Spring::Spring(
	MESH_TYPE meshType,
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 rotation,
	DirectX::XMFLOAT3 scale,
	DirectX::XMFLOAT3 velocity,
	DirectX::XMFLOAT3 launchTarget,
	bool isActive,
	ColliderType colliderType,
	DirectX::XMFLOAT3 collisionBoxSize,
	bool collisionIsTrigger
) : ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::SPRING, COLLISION_LAYER::SPRING)
, m_launchTarget(launchTarget)
{
	m_isDrawn = true;

	m_pColliderSet->AddCollider(
		ColliderType::BOX,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		scale,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
}

// 更新
void Spring::UpdateOverride()
{
}

// 衝突解決
void Spring::ResolveCollisionsOverride()
{
}
