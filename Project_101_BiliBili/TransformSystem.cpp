//プレイヤーに関する関数は全部存在してないため、コード全部コメントにしました

#include "TransformSystem.h"
#include "CollisionManager.h"

void TransformSystem::ApplyTransform(Player* player)
{
    //if (!player)
    //{
    //    return;
    //}

    //// 重複変身を防止
    //for (auto& t : m_transformedPlayers)
    //{
    //    if (t.player == player)
    //    {
    //        return;
    //    }
    //}

    //// 変身開始
    //player->SetTransformed(true);
    //player->SetSpeedMultiplier(0.3f);
    //player->DisableShooting(true);

    //TransformData data;
    //data.player = player;
    //data.timer = 10.0f;

    //m_transformedPlayers.push_back(data);
}

void TransformSystem::Update(float deltaTime)
{
    //for (int i = 0; i < m_transformedPlayers.size(); i++)
    //{
    //    auto& data = m_transformedPlayers[i];

    //     //タイマー減少
    //    data.timer -= deltaTime;

    //    // レーザー更新
    //    UpdateLaser(data);

    //    // 解除
    //    if (data.timer <= 0)
    //    {
    //        data.player->SetTransformed(false);
    //        data.player->SetSpeedMultiplier(1.0f);
    //        data.player->DisableShooting(false);

    //        m_transformedPlayers.erase(m_transformedPlayers.begin() + i);
    //        continue;
    //    }
    //}
}

void TransformSystem::UpdateLaser(TransformData& data)
{
    //Player* p = data.player;

    //Player* teammate = p->GetTeamMate();  // チームの相棒取得
    //if (!teammate)
    //{
    //    return;
    //}

    //Player* hitEnemy = nullptr;
    //if (RayHitEnemy(p, teammate, hitEnemy))
    //{
    //    hitEnemy->ApplyDamage(1.0f);
    //}

}

bool TransformSystem::RayHitEnemy(Player* from, Player* to, Player*& hitEnemy)
{
    //DirectX::XMFLOAT3 start = from->GetPosition();
    //DirectX::XMFLOAT3 end = to->GetPosition();
    //auto result = CollisionManager::Raycast(start, end);

    //if (result.hit && result.object->GetTag() == OBJECT_TAG::PLAYER)
    //{
    //    Player* enemy = dynamic_cast<Player*>(result.object);
    //    if (enemy && enemy->GetTeamID() != from->GetTeamID())
    //    {
    //        hitEnemy = enemy;
    //        return true;
    //    }
    //}
    return false;
}