#include "Item.h"
#include "EventManager.h"
#include "Player.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
Item::Item(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::ITEM_TRANSFORM, colliderType, COLLISION_LAYER::ITEM_TRANSFORM, collisionBoxSize, collisionIsTrigger)
{
	m_isDrawn = true;
}

//更新
void Item::UpdateOverride()
{
}

//衝突解決
void Item::ResolveCollisionsOverride()
{
	auto& infos = m_pCollider->GetCollisionInfos();

	for (auto& info : infos)
	{
		if (info.opponent->GetOwner()->GetTag() == OBJECT_TAG::PLAYER)
		{
			SetActive(false);
			m_pCollider->SetDeleteFlag(true);

			if (Player *player = dynamic_cast<Player *>(info.opponent->GetOwner()))
			{
				//EventManager::GetInstance()->itemPickup[player->GetTeamID()] = true;
				EventManager::GetInstance()->TriggerEvent<int>(EventType::ITEM_PICKUP, player->GetTeamID());
			}
		}
	}
}