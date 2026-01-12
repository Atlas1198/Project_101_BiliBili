#include "BulletManager.h"
#include "CollisionManager.h"
#include "Renderer.h"
#include "EventManager.h"
#include "EffectData.h"

using namespace DirectX;

void BulletManager::FireBullet(
    const DirectX::XMFLOAT3& position,
    const DirectX::XMFLOAT3& direction,
    float speed,
    int ownerTeam,
    uint32_t ownerID
    )
{
    if (teamBulletCount[ownerTeam] <= 0)
    {
        return; // 弾が撃てない場合は何もしない
	}

	teamBulletCount[ownerTeam]--;

	EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>
        (EventType::UPDATE_BULLET_UI, { ownerTeam, teamBulletCount[ownerTeam] });

    auto bullet = std::make_unique<Bullet>(position, direction, speed, ownerTeam, ownerID, BULLET_DAMAGE);
    if (m_pCollisionManager)
    {
		bullet->GetColliderSet()->RegisterColliders(*m_pCollisionManager);
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

	m_restoreModifier = 1.0f + m_totalTimer.Peek() / 60.0f; // ゲーム経過時間に応じて回復速度を上げる


    if (m_bulletRestoreElapsed >= BULLET_RECOVERY / m_restoreModifier)
    {
        for (int team = 0; team < 2; ++team)
        {
            if (teamBulletCount[team] < MAX_BULLETS_PER_TEAM)
            {
                teamBulletCount[team]++;

                EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>
                    (EventType::UPDATE_BULLET_UI, { team, teamBulletCount[team] });
            }

        }

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
    //描画情報生成関数を呼び出し、描画情報を作成
    CreateRenderInfo(
        textureManager,					    //テクスチャマネージャへの参照
        meshManager,					    //メッシュマネージャへの参照
        &m_bulletInfo,					    //描画情報構造体配列へのポインタ
        MESH_TYPE::QUAD,	                //メッシュタイプ
        BLEND_MODE::BLEND_MASKED,		    //ブレンドモード
        texPath,						    //テクスチャのファイル名
		false,							    //ライト無効
		BILLBOARD_TYPE::BILLBOARD_SPHERICAL	//ビルボードタイプ
    );
}