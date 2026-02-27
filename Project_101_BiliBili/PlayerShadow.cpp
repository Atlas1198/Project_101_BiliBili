#include "PlayerShadow.h"
#include "Player.h"

PlayerShadow::PlayerShadow(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, Player* pPlayer, bool collisionIsTrigger)
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
	m_pPlayer = pPlayer;
}

void PlayerShadow::UpdateOverride()
{
	if (!m_pPlayer)
		return;
	// プレイヤーの位置に影を追従させる
	DirectX::XMFLOAT3 playerPos = m_pPlayer->GetPosition();
	m_position.x = playerPos.x;
	m_scale.x = std::max(0.5f, 7.5f - (playerPos.y - m_position.y) * 0.2f);
	m_scale.y = std::max(0.5f, 4.5f - (playerPos.y - m_position.y) * 0.1f);
	m_scale.z = std::max(0.5f, 7.5f - (playerPos.y - m_position.y) * 0.2f);
	m_position.z = playerPos.z;
}
