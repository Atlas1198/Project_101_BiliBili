#pragma once
#include "ObjectManagerBase.h"
#include "Bullet.h"
#include <vector>
#include <memory>
#include "SharedStruct.h"
#include "GameUIManager.h"
#include "FrameTimer.h"

class BulletManager : public ObjectManagerBase
{
public:
    BulletManager() {}
    ~BulletManager() {}

    const wchar_t *texPath = L"asset/texture/bullet.png";

    void FireBullet(
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& direction,
        float speed,
        int ownerTeam,
        uint32_t ownerID);

    void SetGameUIManager(GameUIManager* pGameUIManager)
    {
        m_pGameUIManager = pGameUIManager;
	}

    inline static float BULLET_RECOVERY = 2.0f;
    inline static float BULLET_DAMAGE = 0.1f;

	static constexpr int MAX_BULLETS_PER_TEAM = 6;

protected:
    void InitializeOverride(InputManager* pInputManager,
        TextureManager& textureManager,
        MeshManager& meshManager,
        CollisionManager& collisionManager) override;

    void UpdateOverride() override;
    void SubmitDrawsOverride(Renderer& renderer) override;
    void ResolveCollisionsOverride() override;
    void FinalizeOverride() override;
    void PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager) override;

private:
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    CollisionManager* m_pCollisionManager = nullptr;
    std::vector<RenderData::RenderInfo> m_bulletInfo;
	GameUIManager *m_pGameUIManager = nullptr;
    FrameTimer m_bulletRestoreTimer;
    FrameTimer m_totalTimer;
	float m_bulletRestoreElapsed;
	float m_restoreModifier = 1.0f;

    int teamBulletCount[2] = { MAX_BULLETS_PER_TEAM, MAX_BULLETS_PER_TEAM };
};
