#include "WallPass.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
WallPass::WallPass(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::WALLPASS, colliderType, COLLISION_LAYER::WALLPASS, collisionBoxSize, collisionIsTrigger)
{
}

//更新
void WallPass::UpdateOverride()
{
}

//衝突解決
void WallPass::ResolveCollisionsOverride()
{
}
