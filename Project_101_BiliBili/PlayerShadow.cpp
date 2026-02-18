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
	m_color = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 0.3f); // 半透明の黒色
}

void PlayerShadow::UpdateOverride()
{
	if (!m_pPlayer)
		return;
	// プレイヤーの位置に影を追従させる
	DirectX::XMFLOAT3 playerPos = m_pPlayer->GetPosition();
	m_position.x = playerPos.x;
	//m_position.y = playerPos.y - 0.1f; // 地面に少し浮かせる
	m_position.z = playerPos.z;
}
