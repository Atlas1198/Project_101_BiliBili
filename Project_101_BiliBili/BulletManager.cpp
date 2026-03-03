#include "BulletManager.h"
#include "CollisionManager.h"
#include "Renderer.h"
#include "EventManager.h"
#include "EffectData.h"
#include "AudioManager.h"

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

    auto bullet = std::make_unique<Bullet>(position, direction, speed * m_speedModifier, ownerTeam, ownerID, BULLET_DAMAGE);
    if (m_pCollisionManager)
    {
		bullet->GetColliderSet()->RegisterColliders(*m_pCollisionManager);
    }
    m_bullets.push_back(std::move(bullet));
    AudioManager::GetInstance()->PlaySE("Shoot",0.5f);
}


void BulletManager::InitializeOverride(
    TextureManager& textureManager,
    MeshManager& meshManager,
    CollisionManager& collisionManager
    )
{
    m_pCollisionManager = &collisionManager;

    EventManager::GetInstance()->Subscribe<void>(
        EventType::EVENT_BULLET_SPEED,
        [this](std::shared_ptr<void> data)
        {
            BulletSpeedEvent();
        }
    );

    EventManager::GetInstance()->Subscribe<void>(
        EventType::EVENT_BULLET_RECOVERY,
        [this](std::shared_ptr<void> data)
        {
            BulletRecoveryEvent();
        }
    );

    Reset();
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

    /*if (m_totalTimer.Peek() >= DEFAULT_INCREASE_DELAY)
    {
		defaultSpeedModifier = 1.0f + (m_totalTimer.Peek() - DEFAULT_INCREASE_DELAY) * 0.01f;
		defaultRecoveryModifier = std::min(defaultSpeedModifier, 2.0f);
	}*/


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

void BulletManager::BulletSpeedEvent()
{
    //m_speedModifier += 0.2f;
    m_speedModifier += 0.2f;
}

void BulletManager::BulletRecoveryEvent()
{
    //m_restoreModifier += 0.2f;
    m_restoreModifier += 0.2f;
}


void BulletManager::SubmitDrawsOverride(Renderer& renderer)
{
    for (auto &bullet : m_bullets)
    {
        //描画要求をシーンに提出
        ObjectManagerBase::SubmitRenderInfo(
            renderer,		//シーンの参照
            *bullet,		//ゲームオブジェクト配列の参照
            bullet->GetTeam() == 0 ? m_bulletInfoBlue : m_bulletInfoRed	//プレイヤー描画情報
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
        &m_bulletInfoRed,					    //描画情報構造体配列へのポインタ
        MESH_TYPE::QUAD,	                //メッシュタイプ
        PSO_KEY_MASKED,		                //ブレンドモード
        texPathRed,						    //テクスチャのファイル名
		false,							    //ライト無効
		BILLBOARD_TYPE::BILLBOARD_SPHERICAL	//ビルボードタイプ
    );

    CreateRenderInfo(
        textureManager,					    //テクスチャマネージャへの参照
        meshManager,					    //メッシュマネージャへの参照
        &m_bulletInfoBlue,					//描画情報構造体配列へのポインタ
        MESH_TYPE::QUAD,	                //メッシュタイプ
        PSO_KEY_MASKED,		                //ブレンドモード
        texPathBlue,						//テクスチャのファイル名
        false,							    //ライト無効
        BILLBOARD_TYPE::BILLBOARD_SPHERICAL	//ビルボードタイプ
    );
}

void BulletManager::Reset()
{
    m_bulletRestoreTimer.Mark();
    m_bonusTimer.Mark();
	m_totalTimer.Mark();
    m_bulletRestoreElapsed = 0.0f;
    m_speedModifier = 1.0f;
    m_restoreModifier = 1.0f;
}