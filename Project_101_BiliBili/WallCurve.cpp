#include "WallCurve.h"

using namespace DirectX;
using namespace CollisionData;

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
    : ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::WALLCURVE, COLLISION_LAYER::WALLCURVE)
    , m_rotationSpeed(rotationSpeed)
    , m_orbit(orbit)
    , m_orbitRadius(orbitRadius)
    , m_orbitCenter(orbitCenter)
    , m_forwardOffsetDeg(forwardOffsetDeg)
{
    m_isDrawn = true;

    if (m_orbit)
    {
        XMVECTOR pos = XMLoadFloat3(&m_position);
        XMVECTOR center = XMLoadFloat3(&m_orbitCenter);
        XMVECTOR offset = XMVectorSubtract(pos, center);

        XMFLOAT3 o;
        XMStoreFloat3(&o, offset);

        float radius = sqrtf(o.x * o.x + o.z * o.z);
        if (m_orbitRadius <= 0.0f)
        {
            m_orbitRadius = radius;
        }

        if (radius > 0.0001f)
        {
            m_currentOrbitAngleDeg =
                XMConvertToDegrees(atan2f(o.x, o.z));
        }
    }

}

void WallCurve::UpdateOverride()
{
    if (m_orbit)
    {
        // ---- orbit ˆÚ“® ----
        m_currentOrbitAngleDeg += m_rotationSpeed;
        if (m_currentOrbitAngleDeg >= 360.0f) m_currentOrbitAngleDeg -= 360.0f;
        if (m_currentOrbitAngleDeg < 0.0f)    m_currentOrbitAngleDeg += 360.0f;

        float rad = XMConvertToRadians(m_currentOrbitAngleDeg);

        m_position.x = m_orbitCenter.x + sinf(rad) * m_orbitRadius;
        m_position.z = m_orbitCenter.z + cosf(rad) * m_orbitRadius;

        // ---- ’†S‚ðŒ©‚é ----
        float dx = m_orbitCenter.x - m_position.x;
        float dz = m_orbitCenter.z - m_position.z;

        float yawDeg = XMConvertToDegrees(atan2f(dx, dz));

        // š ³–Ê•â³‚ð‰Á‚¦‚é
        m_rotation.y = yawDeg + m_forwardOffsetDeg;

        // ³‹K‰»
        if (m_rotation.y >= 360.0f) m_rotation.y -= 360.0f;
        if (m_rotation.y < 0.0f)    m_rotation.y += 360.0f;
    }
    else
    {
        // ’Êí‰ñ“]
        if (m_rotationSpeed != 0.0f)
        {
            m_rotation.y += m_rotationSpeed;
            if (m_rotation.y >= 360.0f) m_rotation.y -= 360.0f;
            if (m_rotation.y < 0.0f)    m_rotation.y += 360.0f;
        }
    }
}

void WallCurve::ResolveCollisionsOverride()
{
}
