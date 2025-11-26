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
        //描画要求をシーンに提出
        ObjectManagerBase::SubmitRenderInfo(
            renderer,		//シーンの参照
            *item,		//ゲームオブジェクト配列の参照
            m_itemInfo	//プレイヤー描画情報
        );
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
        //描画情報生成関数を呼び出し、描画情報を作成
        CreateRenderInfo(
            textureManager,					//テクスチャマネージャへの参照
            meshManager,					//メッシュマネージャへの参照
            &m_itemInfo,					//描画情報構造体配列へのポインタ
            m_items[0]->GetMeshType(),	//メッシュタイプ
            BLEND_MODE::BLEND_MASKED,		//ブレンドモード
            texPath							//テクスチャのファイル名
        );
    }
}