#include "TransformItemManager.h"
#include "Renderer.h"

void TransformItemManager::InitializeOverride(InputManager* pInputManager,
    TextureManager& textureManager,
    MeshManager& meshManager,
    CollisionManager& collisionManager)
{
    m_spawnTimer = 20.0f; // 初回出現まで20秒
}

void TransformItemManager::UpdateOverride()
{
    // アイテム出現タイマー
    m_spawnTimer -= 1.0f / 60.0f; // 60FPS
    if (m_spawnTimer <= 0.0f)
    {
        DirectX::XMFLOAT3 spawnPos = { (float)(rand() % 10), 0.0f, (float)(rand() % 10) };
        m_items.push_back(std::make_unique<TransformItem>(spawnPos));
        m_spawnTimer = 20.0f; // リセット
    }

    for (auto& item : m_items)
    {
        if (item->IsActive())
        {
            item->Update();
        }
    }

    m_items.erase(
        std::remove_if(m_items.begin(), m_items.end(),
            [](const std::unique_ptr<TransformItem>& i) { return !i->IsActive(); }),
        m_items.end()
    );
}

void TransformItemManager::SubmitDrawsOverride(Renderer& renderer)
{
    for (auto& item : m_items)
    {
        if (item->IsActive())
        {
            RenderData::RenderInfo info{};
            info.pMeshGPU = nullptr;
            info.world = item->GetWorldMatrix();
            info.positionW = item->GetPosition();
            info.blendMode = BLEND_OPAQUE;

            renderer.Submit(info);
        }
    }
}

void TransformItemManager::ResolveCollisionsOverride()
{
    for (auto& item : m_items)
    {
        if (item->IsActive())
        {
            item->ResolveCollisions();
        }
    }
}

void TransformItemManager::FinalizeOverride()
{
    m_items.clear();
}

void TransformItemManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
    for (auto& item : m_items)
    {
        if (item->IsActive())
        {
            // テクスチャやメッシュを登録
        }
    }
}