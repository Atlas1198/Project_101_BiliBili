#include "NoCollisionModel.h"

using namespace DirectX;
using namespace CollisionData;

// コンストラクタ
NoCollisionModel::NoCollisionModel(MESH_TYPE meshType,
    DirectX::XMFLOAT3 position,
    DirectX::XMFLOAT3 rotation,
    DirectX::XMFLOAT3 scale,
    DirectX::XMFLOAT3 velocity,
    bool isActive,
    ColliderType colliderType,
    DirectX::XMFLOAT3 collisionBoxSize,
    bool collisionIsTrigger,
    float rotationSpeed,
    bool orbit,
    float orbitRadius,
    DirectX::XMFLOAT3 orbitCenter)
    : ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::WALL, COLLISION_LAYER::WALL)
    , m_rotationSpeed(rotationSpeed)
    , m_orbit(orbit)
    , m_orbitRadius(orbitRadius)
    , m_orbitCenter(orbitCenter)
{
    m_isDrawn = true;
}

// 更新
void NoCollisionModel::UpdateOverride()
{
    
}

// 衝突解決
void NoCollisionModel::ResolveCollisionsOverride()
{
}
