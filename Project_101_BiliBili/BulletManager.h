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

    const wchar_t *texPathRed = L"asset/texture/game_scene/ball_R_EF.png";
    const wchar_t *texPathBlue = L"asset/texture/game_scene/ball_B_EF.png";

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
    inline static float BULLET_BONUS_SPEED_MUL = 2.0f;
	inline static float BONUS_RECOVERY_MODIFIER = 1.5f;
	inline static float EVENT_RECOVERY_MODIFIER = 2.0f;

	static constexpr int MAX_BULLETS_PER_TEAM = 6;

protected:
    void InitializeOverride(
        TextureManager& textureManager,
        MeshManager& meshManager,
        CollisionManager& collisionManager) override;

    void UpdateOverride() override;
    void SubmitDrawsOverride(Renderer& renderer) override;
    void ResolveCollisionsOverride() override;
    void FinalizeOverride() override;
    void PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager) override;
    void Reset();

private:
	void BulletSpeedEvent();
	void BulletRecoveryEvent();

private:
    std::vector<std::unique_ptr<Bullet>> m_bullets;
    CollisionManager* m_pCollisionManager = nullptr;
    std::vector<WorldRenderInfo> m_bulletInfoRed;
    std::vector<WorldRenderInfo> m_bulletInfoBlue;
	GameUIManager *m_pGameUIManager = nullptr;
    FrameTimer m_bulletRestoreTimer;
    FrameTimer m_totalTimer;
	float m_bulletRestoreElapsed;
	float m_currentRestoreModifier = 1.0f;
	float m_normalRestoreModifier = 1.0f;
    float m_speedModifier = 1.0f;
    float timeUntilBonusRestoreModifier = 75.0f;

    int teamBulletCount[2] = { MAX_BULLETS_PER_TEAM, MAX_BULLETS_PER_TEAM };
};
