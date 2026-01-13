#include "BilibiliArea.h"
#include <cmath>
#include "EventManager.h"
#include "EffectData.h"
#include "Player.h"

using namespace DirectX;

BilibiliArea::BilibiliArea(
    const XMFLOAT3 &pos,
    int ownerTeam,
    float damage)

    : ObjectBase(
        MESH_TYPE::CUBE,
        pos,
        { 0,0,0 },
        { 2.0f,2.0f,2.0f },
        { 0,0,0 },
        true,
        OBJECT_TAG::BULLET,
        CollisionData::COLLISION_LAYER::BULLET
    ), m_ownerTeam(ownerTeam), m_damage(damage)
{
    SetActive(true);

    m_pColliderSet->AddCollider(
        ColliderType::BOX,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(3.0f, 3.0f, 3.0f),
        XMFLOAT3(0.0f, 0.0f, 0.0f)
    );
}

//プレイヤー位置の設定
void BilibiliArea::SetPlayerPos(const DirectX::XMFLOAT3& position)
{
    m_position = position;
}

void SetBB(int teamID, bool active)
{

}



void BilibiliArea::UpdateOverride()
{
    if (!IsActive())
    {
        return;
    }

    SetPosition(m_position);

    // 前回位置
    //DirectX::XMFLOAT3 oldPos = m_position;

    //// 移動
    //m_position.x += m_direction.x * m_speed;
    //m_position.y += m_direction.y * m_speed;
    //m_position.z += m_direction.z * m_speed;

    //// 距離加算
    //float dx = m_position.x - oldPos.x;
    //float dy = m_position.y - oldPos.y;
    //float dz = m_position.z - oldPos.z;
    //m_traveled += sqrtf(dx * dx + dy * dy + dz * dz);

    //SetPosition(m_position);

    ////弾の存在時間
    //m_livedTime += 1.0f / 60.0f;
    //if (m_livedTime >= m_lifeTime || m_traveled >= m_maxDistance)
    //{
    //    m_deleteFlag = true;
    //    SetActive(false);
    //}
}


void BilibiliArea::ResolveCollisionsOverride()
{
    if (!IsActive())
    {
        return;
    }

    auto &infos = m_pColliderSet->GetCollisionInfos();

    for (const auto &info : infos)
    {
        ObjectBase *otherOwner = info.opponent;

        if (!otherOwner)
        {
            continue;
        }

        if (Player *otherPlayer = dynamic_cast<Player *>(otherOwner))
        {
            if (otherPlayer->GetTeamID() != m_ownerTeam)
            {
                EventManager::GetInstance()->TriggerEvent<std::pair<int, float>>(
                    EventType::TAKE_DAMAGE,
                    std::make_pair(otherPlayer->GetTeamID(), m_damage)
                );
            }
        }
    }

    GetColliderSet()->ClearCollisionInfos();
}