#pragma once

#include "ObjectBase.h"
#include "RenderData.h"

class CollisionManager;
class MeshManager;
class Renderer;
class TextureManager;

// Stationary target that can be damaged by player bullets.
class Enemy final : public ObjectBase
{
public:
	// Number of bullets the enemy can take before disappearing.
	static constexpr int MAX_HP = 5;
	static constexpr int RESPAWN_FRAMES = 120;

	explicit Enemy(const DirectX::XMFLOAT3& position);

	void Initialize(TextureManager& textureManager, MeshManager& meshManager,
		CollisionManager& collisionManager);
	void SubmitDraw(Renderer& renderer) const;
	void TakeDamage();
	void TakeBilibiliDamage(float damage);

	int GetHP() const { return m_hp; }
	bool IsDead() const { return m_hp <= 0; }

private:
	void UpdateOverride() override;
	void ResolveCollisionsOverride() override;

	int m_hp = MAX_HP;
	int m_damageFlashFrames = 0;
	int m_respawnFrames = 0;
	float m_bilibiliDamageAccumulator = 0.0f;
	std::vector<WorldRenderInfo> m_renderInfo;
};
