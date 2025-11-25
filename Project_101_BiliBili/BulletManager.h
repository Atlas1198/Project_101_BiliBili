#pragma once
#include "ObjectManagerBase.h"
#include "Bullet.h"
#include <vector>
#include <memory>

class BulletManager : public ObjectManagerBase
{
public:
    BulletManager() {}
    ~BulletManager() {}

    void FireBullet(
        const DirectX::XMFLOAT3& position,
        const DirectX::XMFLOAT3& direction,
        float speed,
        int ownerTeam);

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
};
