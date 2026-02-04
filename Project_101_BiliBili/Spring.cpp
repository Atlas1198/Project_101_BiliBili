#include "Spring.h"
#include "EventManager.h"
#include "EventType.h"
#include "EffectData.h"
#include "Debug.h"
#include <chrono>

using namespace DirectX;

Spring::Spring(
    MESH_TYPE meshType,
    XMFLOAT3 position,
    XMFLOAT3 rotation,
    XMFLOAT3 scale,
    XMFLOAT3 velocity,
    XMFLOAT3 launchTarget,
    bool isActive,
    ColliderType colliderType,
    XMFLOAT3 collisionBoxSize,
    bool collisionIsTrigger
)
    : ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::SPRING, CollisionData::COLLISION_LAYER::SPRING)
{
    // 乱数初期化
    const auto seed = static_cast<unsigned>(
        std::chrono::high_resolution_clock::now().time_since_epoch().count()
        );
    m_rng.seed(seed);

    // デフォルトは「単発ターゲット」
    m_launchTargets.clear();
    m_launchTargets.push_back(launchTarget);
    m_randomLaunch = false;

    const float scaleFacttor = 1.5f;
    XMFLOAT3 colliderScale =
    {
        scale.x * scaleFacttor,
        scale.y * scaleFacttor,
        scale.z * scaleFacttor,
    };

    // Collider作るならここ（あなたのSpringの実装に合わせて）
    m_pColliderSet->AddCollider(
        ColliderType::BOX,
        DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f),
        colliderScale,
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
    );

    XMFLOAT3 windPosition =
    {
        m_position.x,
		m_position.y +2.5f,
		m_position.z,
    };

    EventManager::GetInstance()->TriggerEvent<EffectCommand>(
        EventType::ADD_EFFECT,
        EffectCommand{
            EFFECT_TYPE::WIND,
            windPosition,
            XMFLOAT2{ 5.0f,5.0f },
        }
        );
}

void Spring::SetLaunchTarget(const XMFLOAT3& target)
{
    m_launchTargets.clear();
    m_launchTargets.push_back(target);
    m_randomLaunch = false;
}

void Spring::SetLaunchTargets(const std::vector<XMFLOAT3>& targets, bool random)
{
    m_launchTargets = targets;
    m_randomLaunch = random;
}

XMFLOAT3 Spring::ChooseLaunchTarget() const
{
    if (m_launchTargets.empty())
    {
        // 念のため：未設定なら自分の位置へ（飛ばない）
        return GetPosition();
    }

    if (!m_randomLaunch || m_launchTargets.size() == 1)
    {
        return m_launchTargets[0];
    }

    std::uniform_int_distribution<size_t> dist(0, m_launchTargets.size() - 1);
    return m_launchTargets[dist(m_rng)];
}

void Spring::UpdateOverride() 
{
  m_nodeAnimatorSet.isAnimPlaying = true;

	//アニメーション更新
	const double MAX_ANIM_TIME = 1.0 / 30.0; //slowest speed
	if (m_isBlowing)
	{
		const double MAX_ANIM_TIME = 1.0 / 30.0; //slowest speed
		m_animTime = (std::min)(m_animTime * 1.10, MAX_ANIM_TIME); //fast speed

		m_blowCount++;
		const float BLOW_DURATION = 60.0f; //frames
		if (m_blowCount > BLOW_DURATION)
		{
			m_isBlowing = false;
			m_blowCount = 0;
		}
	}
    else
    {
        m_animTime = (std::max)(m_animTime * 0.98, ANIM_TIME); //fast speed
    }

    m_nodeAnimatorSet.pNodeAnimator->Update(m_animTime);
}
void Spring::ResolveCollisionsOverride() {}
