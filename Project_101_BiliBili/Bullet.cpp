#include "Bullet.h"
#include <cmath>
#include "Player.h"
#include "EventManager.h"
#include "EffectData.h"
#include "AudioManager.h"

using namespace DirectX;

Bullet::Bullet(
               const XMFLOAT3& pos,
               const XMFLOAT3& dir,
               float speed,
               int ownerTeam,
	           uint32_t ownerID,
               float damage,
               float lifeTimeSec,
               float maxDistance)

    : ObjectBase(
        MESH_TYPE::QUAD,
        pos,
        {0,0,0},
        {2.5f,2.5f,2.5f}, 
        {0,0,0},
        true,
        OBJECT_TAG::BULLET,
        CollisionData::COLLISION_LAYER::BULLET
	), m_direction(dir), m_speed(speed), m_ownerTeam(ownerTeam), m_ownerID(ownerID),
	m_lifeTime(lifeTimeSec), m_maxDistance(maxDistance), m_damage(damage)
{
    SetActive(true);

	const XMFLOAT3 COLLIDER_SCALE =
    {
        m_scale.x * 0.6f,
        m_scale.y * 0.6f,
		m_scale.z * 0.6f
    };

    m_pColliderSet->AddCollider(
        ColliderType::SPHERE,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        COLLIDER_SCALE,
        XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

    TexSplitInfo texInfo{};
    texInfo.cols = 6;
    texInfo.rows = 5;
    texInfo.total = texInfo.cols * texInfo.rows;
    texInfo.frameCount = 0;
    texInfo.updateRate = 2;
    m_texSplitInfo = texInfo;
}


void Bullet::UpdateOverride()
{
    if (!IsActive()) 
    {
        return;
    }

    // 前回位置
    DirectX::XMFLOAT3 oldPos = m_position;

    // 移動
    m_position.x += m_direction.x * m_speed;
    m_position.y += m_direction.y * m_speed;
    m_position.z += m_direction.z * m_speed;

    // 距離加算
    float dx = m_position.x - oldPos.x;
    float dy = m_position.y - oldPos.y;
    float dz = m_position.z - oldPos.z;
    m_traveled += sqrtf(dx * dx + dy * dy + dz * dz);

    SetPosition(m_position);

    //弾の存在時間
    m_livedTime += 1.0f / 60.0f;
    if (m_livedTime >= m_lifeTime || m_traveled >= m_maxDistance)
    {
        m_deleteFlag = true;
        SetActive(false);
    }
}


void Bullet::ResolveCollisionsOverride()
{
    if (!IsActive()) 
    {
        return;
    }

    auto& infos = m_pColliderSet->GetCollisionInfos();

    for (const auto& info : infos)
    {
        ObjectBase* otherOwner = info.opponent;

        if (!otherOwner) 
        {
            continue;
        }

        if (Player *otherPlayer = dynamic_cast<Player *>(otherOwner))
		{//プレイヤーに当たった場合
            if (otherPlayer->id == m_ownerID)
			{//自チームには当たらない
                continue;
			}
            else if (otherPlayer->GetTeamID() != m_ownerTeam)
			{//敵チームに当たった場合ダメージを与える
                EventManager::GetInstance()->TriggerEvent<std::pair<int, float>>(
                    EventType::TAKE_DAMAGE,
                    std::make_pair(otherPlayer->GetTeamID(), m_damage)
				);

				otherPlayer->StartDamageAnimation();

                EventManager::GetInstance()->TriggerEvent<EffectCommand>(
                    EventType::ADD_EFFECT,
                    EffectCommand{
                        EFFECT_TYPE::EXPLOSION,
                        m_position,
                        XMFLOAT3{ 2.5f,2.5f,1.0f },
                    }
                    );
                //コントローラー振動
                otherPlayer->ShakeController(1.0f, 1.0f, 20);
            }
        }
        else
        {
            //消滅
            EventManager::GetInstance()->TriggerEvent<EffectCommand>(
                EventType::ADD_EFFECT,
                EffectCommand{
                    EFFECT_TYPE::FIRE_FLASH,
                    m_position,
                    XMFLOAT3{ 2.5f,2.5f,1.0f },
                }
                );
        }

        m_deleteFlag = true;
        SetActive(false);
        break;
    }

    GetColliderSet()->ClearCollisionInfos();
}