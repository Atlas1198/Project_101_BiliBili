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
    float orbitRadius,
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

    // 指定半径を必ず採用（0は事故るので最低値を持たせる）
    m_orbitRadius = (orbitRadius > 0.0001f) ? orbitRadius : 0.0001f;

    // 初期角度：中心→初期位置の方向から決める
    const float ox = m_position.x - m_orbitCenter.x;
    const float oz = m_position.z - m_orbitCenter.z;

    if ((ox * ox + oz * oz) > 0.0001f)
    {
        // あなたの座標系（x=sin, z=cos）に合わせて atan2(ox, oz)
        m_currentOrbitAngleDeg = NormalizeDeg(XMConvertToDegrees(std::atan2f(ox, oz)));
    }
    else
    {
        // 初期位置が中心と同じなら角度0スタート
        m_currentOrbitAngleDeg = 0.0f;
    }

    // ※ここで即座に円周上に置きたいなら、下の行を呼んでもOK（任意）
    // UpdateOverride(); // 初期位置を半径の円周に補正して開始
}

void WallCurve::UpdateOverride()
{
    if (m_orbit)
    {
        m_currentOrbitAngleDeg = NormalizeDeg(m_currentOrbitAngleDeg + m_rotationSpeed);
        const float rad = XMConvertToRadians(m_currentOrbitAngleDeg);

        // 中心から「指定半径」ぶんの円周上を回る
        m_position.x = m_orbitCenter.x + std::sinf(rad) * m_orbitRadius;
        m_position.z = m_orbitCenter.z + std::cosf(rad) * m_orbitRadius;

        // 中心を見る（yaw）
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
