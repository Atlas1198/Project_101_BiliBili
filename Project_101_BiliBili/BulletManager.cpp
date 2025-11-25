#include "BulletManager.h"
#include "CollisionManager.h"
#include "Renderer.h"

void BulletManager::FireBullet(
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& direction,
    float speed,
    int ownerTeam
    )
{
    auto bullet = std::make_unique<Bullet>(position, direction, speed, ownerTeam);
    if (m_pCollisionManager)
    {
        m_pCollisionManager->RegisterCollider(bullet->GetCollider());
    }
    m_bullets.push_back(std::move(bullet));
}


void BulletManager::InitializeOverride(
    InputManager* pInputManager,
    TextureManager& textureManager,
    MeshManager& meshManager,
    CollisionManager& collisionManager
    )
{
    m_pCollisionManager = &collisionManager;
}


void BulletManager::UpdateOverride()
{
    for (auto& b : m_bullets)
    {
        if (b->IsActive())
        {
            b->Update();
        }
    }

    // 弾の削除
    m_bullets.erase(
        std::remove_if
        ( m_bullets.begin(), m_bullets.end(),
            [](const std::unique_ptr<Bullet>& b)        //[キャプチャリスト](引数リスト)   キャプチャリストが空。外部変数をラムダの中で使わないこと
            {
                return (!b->IsActive() || b->DeleteFlag());
            }
        ),
        m_bullets.end()
    );
}


void BulletManager::SubmitDrawsOverride(Renderer& renderer)
{
    for (auto& b : m_bullets)
    {
        if (!b->IsActive())
        {
            continue;
        }

        RenderData::RenderInfo info{};

        DirectX::XMMATRIX world = DirectX::XMMatrixTranslation(
            b->GetPosition().x,
            b->GetPosition().y,
            b->GetPosition().z
        );
        info.world = world;
        info.pMeshGPU = nullptr;
        info.srvIndex = UINT32_MAX;
        info.color = { 1.0f, 1.0f, 1.0f, 1.0f };
        info.positionW = b->GetPosition();
        info.blendMode = BLEND_OPAQUE;

        renderer.Submit(info);
    }
}

void BulletManager::ResolveCollisionsOverride()
{
    for (auto& b : m_bullets)
    {
        if (b->IsActive())
        {
            b->ResolveCollisions();
        }
    }
}

void BulletManager::FinalizeOverride()
{
    m_bullets.clear();
}

void BulletManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
    // テクスチャやメッシュを登録
}