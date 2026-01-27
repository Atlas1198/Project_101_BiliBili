#include "Spring.h"
#include "EventManager.h"
#include "Debug.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
Spring::Spring(MESH_TYPE meshType, 
	DirectX::XMFLOAT3 position, 
	DirectX::XMFLOAT3 rotation, 
	DirectX::XMFLOAT3 scale, 
	DirectX::XMFLOAT3 velocity, 
	bool isActive, 
	ColliderType colliderType, 
	DirectX::XMFLOAT3 collisionBoxSize, 
	bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::SPRING, COLLISION_LAYER::SPRING)
{
	m_isDrawn = true;
	float scaleFactor = 1.5f;
	XMFLOAT3 colliderScale = 
	{
		scale.x * scaleFactor,
		scale.y * scaleFactor,
		scale.z * scaleFactor
	};

	m_pColliderSet->AddCollider(
		ColliderType::BOX,
		DirectX::XMFLOAT3(0.0f, 2.0f, 0.0f),
		colliderScale,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
}

//更新
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
		m_animTime = (std::max)(m_animTime * 0.90, ANIM_TIME); //slow speed
	}
	m_nodeAnimatorSet.pNodeAnimator->Update(m_animTime);
}

//衝突解決
void Spring::ResolveCollisionsOverride()
{
}