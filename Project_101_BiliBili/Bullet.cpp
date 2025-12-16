#include "Bullet.h"
#include <cmath>
#include "Player.h"
#include "EventManager.h"
#include "EffectData.h"

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
        {1.0f,1.0f,1.0f}, 
        {0,0,0},
        true,
        OBJECT_TAG::BULLET,
        CollisionData::COLLISION_LAYER::BULLET
	), m_direction(dir), m_speed(speed), m_ownerTeam(ownerTeam), m_ownerID(ownerID),
	m_lifeTime(lifeTimeSec), m_maxDistance(maxDistance), m_damage(damage)
{
    SetActive(true);

    m_pColliderSet->AddCollider(
        ColliderType::SPHERE,
        XMFLOAT3(0.0f, 0.0f, 0.0f),
        XMFLOAT3(0.5f, 0.5f, 0.5f),
        XMFLOAT3(0.0f, 0.0f, 0.0f)
	);
}


void Bullet::UpdateOverride()
{
    if (!IsActive()) 
    {
        return;
    }

    // ‘O‰ñˆÊ’u
    DirectX::XMFLOAT3 oldPos = m_position;

    // ˆÚ“®
    m_position.x += m_direction.x * m_speed;
    m_position.y += m_direction.y * m_speed;
    m_position.z += m_direction.z * m_speed;

    // ‹——£‰ÁŽZ
    float dx = m_position.x - oldPos.x;
    float dy = m_position.y - oldPos.y;
    float dz = m_position.z - oldPos.z;
    m_traveled += sqrtf(dx * dx + dy * dy + dz * dz);

    SetPosition(m_position);

    //’e‚Ì‘¶ÝŽžŠÔ
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
        {
            if (otherPlayer->id == m_ownerID)
            {
                continue;
			}
            else if (otherPlayer->GetTeamID() != m_ownerTeam)
            {
				//m_pGameUIManager->TakeDamage(otherPlayer->GetTeamID(), 0.1f);
                //EventManager::GetInstance()->TakeDamage(otherPlayer->GetTeamID(), m_damage);
                EventManager::GetInstance()->TriggerEvent<std::pair<int, float>>(
                    EventType::TAKE_DAMAGE,
                    std::make_pair(otherPlayer->GetTeamID(), m_damage)
				);
            }
        }
        else
        {

        }

        // “¯‚¶ƒ`[ƒ€’e‚Í–³Ž‹
        //if (otherOwner->GetTeam() == m_ownerTeam)
        //{
        //    continue;
        //}

        // Õ“Ë ¨ Á–Å
        EventManager::GetInstance()->TriggerEvent<EffectCommand>(
            EventType::ADD_EFFECT,
            EffectCommand{
                EFFECT_TYPE::FIRE_FLASH,
                m_position,
                XMFLOAT2{ 2.5f,2.5f },
            }
        );
        m_deleteFlag = true;
        SetActive(false);
        break;
    }

    GetColliderSet()->ClearCollisionInfos();
}