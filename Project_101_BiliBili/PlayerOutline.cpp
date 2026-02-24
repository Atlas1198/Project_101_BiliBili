#include "PlayerOutline.h"
#include "Player.h"

PlayerOutline::PlayerOutline(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
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
}

void PlayerOutline::UpdateOverride()
{
}
