#include "Wall.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
Wall::Wall(MeshData::MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::WALL, colliderType, COLLISION_LAYER::WALL, collisionBoxSize, collisionIsTrigger)
{
	m_isDrawn = false;
}

//更新
void Wall::UpdateOverride()
{
}

//衝突解決
void Wall::ResolveCollisionsOverride()
{
}
