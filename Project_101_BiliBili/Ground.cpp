#include "Ground.h"

using namespace DirectX;

//コンストラクタ
Ground::Ground(MeshData::MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::GROUND, colliderType, collisionBoxSize, collisionIsTrigger)
{
}

//更新
void Ground::UpdateOverride()
{
}

//衝突解決
void Ground::ResolveCollisionsOverride()
{
}
