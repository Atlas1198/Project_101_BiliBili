#include "Item.h"
#include "EventManager.h"
#include "Player.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
Item::Item(MESH_TYPE meshType, DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 velocity, bool isActive, ColliderType colliderType, DirectX::XMFLOAT3 collisionBoxSize, bool collisionIsTrigger)
	: ObjectBase(meshType, position, rotation, scale, velocity, isActive, OBJECT_TAG::ITEM_TRANSFORM, COLLISION_LAYER::ITEM_TRANSFORM)
{
	m_isDrawn = true;
	XMFLOAT3 colliderSetScale = { 
		scale.x * 0.8f, 
		scale.y * 0.8f, 
		scale.z * 0.8f 
	};

	m_pColliderSet->AddCollider(
		colliderType,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),
		colliderSetScale,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	TexSplitInfo texInfo{};
	texInfo.cols = 6;
	texInfo.rows = 5;
	texInfo.total = texInfo.cols * texInfo.rows;
	texInfo.index = 0;
	texInfo.frameCount = 0;
	texInfo.updateRate = 2;
	m_texSplitInfo = texInfo;
}

//更新
void Item::UpdateOverride()
{
}

//衝突解決
void Item::ResolveCollisionsOverride()
{
	auto& infos = m_pColliderSet->GetCollisionInfos();

	for (auto& info : infos)
	{
		if (info.opponent && info.opponent->GetTag() == OBJECT_TAG::PLAYER)
		{
			SetActive(false);
			m_pColliderSet->SetDeleteFlag(true);

			if (Player *player = dynamic_cast<Player *>(info.opponent))
			{
				//EventManager::GetInstance()->itemPickup[player->GetTeamID()] = true;
				EventManager::GetInstance()->TriggerEvent<int>(EventType::ITEM_PICKUP, player->GetTeamID());
				//アイテム取得エフェクトの発生
				EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(EventType::BB_CUT_IN, std::make_pair(player->GetTeamID(), player->GetCharacterID()));
			}
		}
	}
}