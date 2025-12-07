#include "Wall.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
Wall::Wall(MeshData::MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger, float rotationSpeed)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::WALL, colliderType, COLLISION_LAYER::WALL, collisionBoxSize, collisionIsTrigger),
	m_rotationSpeed(rotationSpeed)
{
	m_isDrawn = true;
}

//更新
void Wall::UpdateOverride()
{
	//回転速度が0でない場合、回転を更新
	if (m_rotationSpeed != 0.0f)
	{
		m_rotation.y += m_rotationSpeed;	//Y軸回転

		//回転角度の補正
		if (m_rotation.y > 360.0f)
		{
			m_rotation.y -= 360.0f;
		}
		if (m_rotation.y < 0.0f) 
		{
			m_rotation.y += 360.0f;
		}
	}
}

//衝突解決
void Wall::ResolveCollisionsOverride()
{
}
