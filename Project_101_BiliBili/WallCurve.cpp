#include "WallCurve.h"
#include <cmath>

using namespace DirectX;
using namespace CollisionData;

namespace
{
    inline float NormalizeDeg(float deg)
    {
        deg = std::fmod(deg, 360.0f);
        if (deg < 0.0f) deg += 360.0f;
        return deg;
    }
}

WallCurve::WallCurve(
    MESH_TYPE meshType,
    XMFLOAT3 position,
    XMFLOAT3 rotation,
    XMFLOAT3 scale,
    XMFLOAT3 velocity,
    bool isActive,
    ColliderType colliderType,
    XMFLOAT3 collisionBoxSize,
    bool collisionIsTrigger,
    float rotationSpeed,
    bool orbit,
    float orbitRadius,          // ← 半径の“保険”としてだけ使う
    XMFLOAT3 orbitCenter,
    float forwardOffsetDeg
)
    : ObjectBase(meshType, position, rotation, scale, velocity, isActive,
        OBJECT_TAG::WALLCURVE, COLLISION_LAYER::WALLCURVE)
    , m_rotationSpeed(rotationSpeed)
    , m_orbit(orbit)
    , m_orbitRadius(0.0f)
    , m_orbitCenter(orbitCenter)
    , m_forwardOffsetDeg(forwardOffsetDeg)
{
    m_isDrawn = true;

    if (!m_orbit)
        return;

    // 初期位置から中心へのオフセット
    const float ox = m_position.x - m_orbitCenter.x;
    const float oz = m_position.z - m_orbitCenter.z;

    // 半径は “中心から初期位置までの距離” を必ず採用
    const float r = std::sqrt(ox * ox + oz * oz);

    if (r > 0.0001f)
    {
        m_orbitRadius = r;
        m_currentOrbitAngleDeg = NormalizeDeg(XMConvertToDegrees(std::atan2f(ox, oz)));
    }
    else
    {
        // 初期位置が中心と同じ＝半径が作れないので、引数を保険として使う
        m_orbitRadius = (orbitRadius > 0.0f) ? orbitRadius : 0.0f;
        m_currentOrbitAngleDeg = 0.0f;
    }
}

void WallCurve::UpdateOverride()
{
    if (m_orbit)
    {
        m_currentOrbitAngleDeg = NormalizeDeg(m_currentOrbitAngleDeg + m_rotationSpeed);
        const float rad = XMConvertToRadians(m_currentOrbitAngleDeg);

        // 回転
        m_position.x = m_orbitCenter.x + std::sinf(rad) * m_orbitRadius;
        m_position.z = m_orbitCenter.z + std::cosf(rad) * m_orbitRadius;

        // 中心を見る
        const float dx = m_orbitCenter.x - m_position.x;
        const float dz = m_orbitCenter.z - m_position.z;

        const float yawDeg = XMConvertToDegrees(std::atan2f(dx, dz));
        m_rotation.y = NormalizeDeg(yawDeg + m_forwardOffsetDeg);
    }
    else
    {
        if (m_rotationSpeed != 0.0f)
            m_rotation.y = NormalizeDeg(m_rotation.y + m_rotationSpeed);
    }
}

void WallCurve::ResolveCollisionsOverride()
{
}
