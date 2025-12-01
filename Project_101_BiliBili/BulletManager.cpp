#include "BulletManager.h"
#include "CollisionManager.h"
#include "Renderer.h"
#include "EventManager.h"

void BulletManager::FireBullet(
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& direction,
    float speed,
    int ownerTeam,
    uint32_t ownerID
    )
{
    auto bullet = std::make_unique<Bullet>(m_pGameUIManager, position, direction, speed, ownerTeam, ownerID);
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

    m_bulletRestoreElapsed += m_bulletRestoreTimer.Mark();

    if (m_bulletRestoreElapsed >= bulletRestoreSeconds)
    {
		EventManager::GetInstance()->AddBullets(1);
        m_bulletRestoreElapsed = 0.0f;
    }
}


void BulletManager::SubmitDrawsOverride(Renderer& renderer)
{
    for (auto &bullet : m_bullets)
    {
        //描画要求をシーンに提出
        ObjectManagerBase::SubmitRenderInfo(
            renderer,		//シーンの参照
            *bullet,		//ゲームオブジェクト配列の参照
            m_bulletInfo	//プレイヤー描画情報
        );
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
    for (auto &bullet : m_bullets)
    {
        //描画情報生成関数を呼び出し、描画情報を作成
        CreateRenderInfo(
            textureManager,					//テクスチャマネージャへの参照
            meshManager,					//メッシュマネージャへの参照
            &m_bulletInfo,					//描画情報構造体配列へのポインタ
            m_bullets[0]->GetMeshType(),	//メッシュタイプ
            BLEND_MODE::BLEND_MASKED,		//ブレンドモード
            texPath							//テクスチャのファイル名
        );
    }
}