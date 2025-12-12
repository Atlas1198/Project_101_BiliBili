#include "Wall.h"

using namespace DirectX;
using namespace CollisionData;

// コンストラクタ
Wall::Wall(MeshData::MESH_TYPE meshType,
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
    : ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::WALL, colliderType, COLLISION_LAYER::WALL, collisionBoxSize, collisionIsTrigger)
    , m_rotationSpeed(rotationSpeed)
    , m_orbit(orbit)
    , m_orbitRadius(orbitRadius)
    , m_orbitCenter(orbitCenter)
{
    m_isDrawn = true;

    // orbit 半径が 0 の場合は、初期位置と orbitCenter から半径を計算する
    if (m_orbit)
    {
        // offset = position - orbitCenter
        XMVECTOR posVec = XMLoadFloat3(&m_position);
        XMVECTOR centerVec = XMLoadFloat3(&m_orbitCenter);
        XMVECTOR offset = XMVectorSubtract(posVec, centerVec);

        // XZ平面の距離を半径とする
        XMFLOAT3 offsetF;
        XMStoreFloat3(&offsetF, offset);
        float dx = offsetF.x;
        float dz = offsetF.z;
        float computedRadius = sqrtf(dx * dx + dz * dz);

        if (m_orbitRadius <= 0.0f)
        {
            m_orbitRadius = computedRadius;
        }

        // 初期角度（度）を計算して保持しておく（atan2 の順に注意）
        // atan2(y, x) -> ここでは atan2(dx, dz) にして、角度=0 の時に z が正の方向になるようにする
        float angleRad = 0.0f;
        if (computedRadius > 0.0001f)
        {
            angleRad = atan2f(dx, dz); // dx = sin(angle)*r, dz = cos(angle)*r
        }
        m_currentOrbitAngleDeg = XMConvertToDegrees(angleRad);
    }
}

// 更新
void Wall::UpdateOverride()
{
    // orbit モード
    if (m_orbit)
    {
        // 回転角を進める（deg/frame）
        m_currentOrbitAngleDeg += m_rotationSpeed;

        // ラップ
        if (m_currentOrbitAngleDeg >= 360.0f) 
        {
            m_currentOrbitAngleDeg -= 360.0f;
        }
        if (m_currentOrbitAngleDeg < 0.0f) 
        {
            m_currentOrbitAngleDeg += 360.0f;
        }

        // 角度をラジアンに変換
        float angleRad = XMConvertToRadians(m_currentOrbitAngleDeg);

        // Y軸周りに orbitRadius で位置を計算（x,z）
        float x = m_orbitCenter.x + sinf(angleRad) * m_orbitRadius;
        float z = m_orbitCenter.z + cosf(angleRad) * m_orbitRadius;
        float y = m_position.y; // Y は元の高さを維持

        m_position.x = x;
        m_position.z = z;
        m_position.y = y;

        //// オブジェクト自体の向き（見た目の回転）も変えたい場合はここで変更
        //if (m_rotationSpeed != 0.0f)
        //{
        //    m_rotation.y += m_rotationSpeed;
        //    if (m_rotation.y >= 360.0f) 
        //    {
        //        m_rotation.y -= 360.0f;
        //    }
        //    if (m_rotation.y < 0.0f) 
        //    {
        //        m_rotation.y += 360.0f;
        //    }
        //}

        // orbit している方向を向くようにする
        float dx = m_orbitCenter.x - m_position.x;
        float dz = m_orbitCenter.z - m_position.z;

        float yawRad = atan2f(dx, dz);
        float yawDeg = XMConvertToDegrees(yawRad);

        m_rotation.y = yawDeg;
    }
    else
    {
        // orbit でない -> 従来どおりの自己回転
        if (m_rotationSpeed != 0.0f)
        {
            m_rotation.y += m_rotationSpeed;    // Y軸回転

            // 回転角度の補正
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
}

// 衝突解決
void Wall::ResolveCollisionsOverride()
{
}
