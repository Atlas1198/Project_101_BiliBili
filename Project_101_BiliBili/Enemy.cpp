#include "Enemy.h"

#include "BulletManager.h"
#include "CollisionManager.h"
#include "MeshManager.h"
#include "Renderer.h"
#include "TextureManager.h"

using namespace DirectX;
using namespace CollisionData;

Enemy::Enemy(const XMFLOAT3& position)
	: ObjectBase(
		MESH_TYPE::QUAD,
		position,
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(5.5f, 5.5f, 5.5f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		true,
		OBJECT_TAG::PLAYER,
		COLLISION_LAYER::PLAYER,
		XMFLOAT3(0.5f, 0.5f, 0.5f))
{
	const XMFLOAT3 colliderScale =
	{
		m_scale.x * 1.2f,
		m_scale.y * 1.2f,
		m_scale.z * 1.2f
	};

	m_pColliderSet->AddCollider(
		ColliderType::SPHERE,
		XMFLOAT3(0.0f, -0.5f, 0.0f),
		colliderScale,
		XMFLOAT3(0.0f, 0.0f, 0.0f));

	TexSplitInfo texInfo{};
	texInfo.cols = 3;
	texInfo.rows = 8;
	texInfo.total = texInfo.cols * texInfo.rows;
	texInfo.index = 0;
	m_texSplitInfo = texInfo;
	m_color = XMFLOAT4(1.0f, 0.25f, 0.25f, 1.0f);
}

void Enemy::Initialize(TextureManager& textureManager, MeshManager& meshManager,
	CollisionManager& collisionManager)
{
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_renderInfo,
		GetMeshType(),
		PSO_KEY_TRANSPARENT,
		L"asset/texture/player/NEON_off_CH.png",
		false,
		BILLBOARD_TYPE::BILLBOARD_FIX_X);

	m_pColliderSet->RegisterColliders(collisionManager);
}

void Enemy::UpdateOverride()
{
	if (IsDead())
	{
		if (m_respawnFrames > 0)
		{
			--m_respawnFrames;
		}

		if (m_respawnFrames == 0)
		{
			m_hp = MAX_HP;
			m_bilibiliDamageAccumulator = 0.0f;
			m_damageFlashFrames = 0;
			m_color = XMFLOAT4(1.0f, 0.25f, 0.25f, 1.0f);
			SetActive(true);
			m_pColliderSet->SetActive(true);
		}

		return;
	}

	if (m_damageFlashFrames > 0)
	{
		--m_damageFlashFrames;
		m_color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	}
	else
	{
		m_color = XMFLOAT4(1.0f, 0.25f, 0.25f, 1.0f);
	}
}

void Enemy::ResolveCollisionsOverride()
{
}

void Enemy::TakeDamage()
{
	if (!IsActive())
	{
		return;
	}

	--m_hp;
	m_damageFlashFrames = 6;

	if (IsDead())
	{
		m_respawnFrames = RESPAWN_FRAMES;
		SetActive(false);
		m_pColliderSet->SetActive(false);
	}
}

void Enemy::TakeBilibiliDamage(float damage)
{
	if (!IsActive() || damage <= 0.0f)
	{
		return;
	}

	m_bilibiliDamageAccumulator += damage;
	while (m_bilibiliDamageAccumulator >= BulletManager::BULLET_DAMAGE && IsActive())
	{
		m_bilibiliDamageAccumulator -= BulletManager::BULLET_DAMAGE;
		TakeDamage();
	}
}

void Enemy::SubmitDraw(Renderer& renderer) const
{
	if (!IsActive() || !IsDrawn())
	{
		return;
	}

	auto submitInfo = m_renderInfo;
	for (auto& info : submitInfo)
	{
		info.world = GetWorldMatrix();
		info.position = GetPosition();
		info.scale = GetScale();
		info.common.color.x *= GetColor().x;
		info.common.color.y *= GetColor().y;
		info.common.color.z *= GetColor().z;
		info.common.color.w *= GetColor().w;
		info.common.uvRect = SplitSprite(GetTexSplitInfo());
	}

	renderer.SubmitToWorldList(submitInfo);
}
