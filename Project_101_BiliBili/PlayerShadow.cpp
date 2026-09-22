#include "PlayerShadow.h"
#include <algorithm>

PlayerShadow::PlayerShadow(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, ObjectBase* pTarget, bool collisionIsTrigger)
	: ObjectBase(
		meshType,
		position,
		rotation,
		scale,
		velocity,
		true,
		OBJECT_TAG::PLAYER,
		CollisionData::COLLISION_LAYER::PLAYER,
		collisionBoxSize
	)
{
	m_pTarget = pTarget;
	m_baseShadowScale = scale;
	m_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f); // 半透明の黒色
}

void PlayerShadow::UpdateOverride()
{
	if (!m_pTarget)
		return;
	// 対象オブジェクトの位置に影を追従させる
	DirectX::XMFLOAT3 targetPos = m_pTarget->GetPosition();
	m_position.x = targetPos.x;
	m_scale.x = std::max(0.5f, m_baseShadowScale.x - (targetPos.y - m_position.y) * 0.2f);
	m_scale.y = std::max(0.5f, m_baseShadowScale.y - (targetPos.y - m_position.y) * 0.1f);
	m_scale.z = std::max(0.5f, m_baseShadowScale.z - (targetPos.y - m_position.y) * 0.2f);
	m_position.z = targetPos.z;
}
