#include "ElectricityBB.h"
#include "SharedStruct.h"
#include "Player.h"

using namespace DirectX;
using namespace CollisionData;

//更新
void ElectricityBB::UpdateOverride()
{
	UpdateGeometry();
}

//衝突解決
void ElectricityBB::ResolveCollisionsOverride()
{
	for(auto& info : m_pColliderSet->GetCollisionInfos())
	{
		auto opponentOwner = info.opponent;			//衝突相手のオブジェクト取得
		OBJECT_TAG tag = opponentOwner->GetTag();	//衝突相手のタグ取得

		//衝突相手がプレイヤーの場合
		if(tag == OBJECT_TAG::PLAYER)
		{
			auto player = dynamic_cast<Player*>(opponentOwner);	//プレイヤーにキャスト
			if (player->GetTeamID() != m_teamId)
			{
				if (player->GetTeamID() != m_teamId && info.state == COLLISION_STATE::COLLISION_STAY)
				{//チームIDが違うか、衝突開始の場合
					m_hasHitPlayer = true;	//プレイヤーに当たったフラグを立てる
					m_hitNum++;				//当たった回数をカウント
				}
				player->ShakeController(1.0f, 1.0f, 20);
				player->StartDamageAnimation();
			}
		}
	}
}

//プレイヤーに当たったかどうか取得
bool ElectricityBB::HasHitPlayer() const
{
	return m_hasHitPlayer;
}

//当たった回数取得
int ElectricityBB::GetHitNum() const
{
	return m_hitNum;
}

//開始地点の設定
void ElectricityBB::SetStartPos(DirectX::XMFLOAT3 position)
{
	m_startPos = position;
}

//終了地点の設定
void ElectricityBB::SetEndPos(DirectX::XMFLOAT3 position)
{
	m_endPos = position;
}

//チームIDの設定
void ElectricityBB::SetTeamId(int id)
{
	m_teamId = id;
}

//プレイヤーに当たったかどうかフラグの設定
void ElectricityBB::SetHasHitPlayer(bool hasHit)
{
	m_hasHitPlayer = hasHit;
}

//当たった回数の設定
void ElectricityBB::SetHitNum(int num)
{
	m_hitNum = num;
}

//ジオメトリ更新
void ElectricityBB::UpdateGeometry()
{
	float length = LengthBetween(m_startPos, m_endPos);
	m_scale.y = length;

	XMFLOAT3 position;
	position.x = (m_startPos.x + m_endPos.x) * 0.5f;
	position.y = (m_startPos.y + m_endPos.y) * 0.5f;
	position.z = (m_startPos.z + m_endPos.z) * 0.5f;
	m_position = position;
}