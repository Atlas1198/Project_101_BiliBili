#include "LineBB.h"
#include <DirectXMath.h>

using namespace DirectX;
using namespace CollisionData;

//更新
void LineBB::UpdateOverride()
{
	//ラインの設定
	SetLine(
		m_edgePos[0],
		m_edgePos[1]
	);
}

//衝突解決
void LineBB::ResolveCollisionsOverride()
{
	//壁との衝突収集
	CollectWallCollisions();
}

//エッジの位置設定
void LineBB::SetEdgePos(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	m_edgePos[0] = start;
	m_edgePos[1] = end;
}

//壁との衝突点取得
std::vector<DirectX::XMFLOAT3> LineBB::GetWallCollisionPoints() const
{
	return m_wallCollisionPoints;
}

//ラインの設定
void LineBB::SetLine(DirectX::XMFLOAT3 start, DirectX::XMFLOAT3 end)
{
	//座標
	XMFLOAT3 midPos{};	//中点座標
	midPos.x = (start.x + end.x) * 0.5f;
	midPos.y = (start.y + end.y) * 0.5f;
	midPos.z = (start.z + end.z) * 0.5f;
	m_position = midPos;	//位置設定

	//長さ
	m_length = sqrtf(
		(start.x - end.x) * (start.x - end.x) +
		(start.y - end.y) * (start.y - end.y) +
		(start.z - end.z) * (start.z - end.z)
	);
	m_scale.y = m_length;	//スケール設定	

	XMFLOAT3 startPosLocal{};	//ローカル座標系での開始位置
	XMFLOAT3 endPosLocal{};		//ローカル座標系での終了位置
	startPosLocal.x = start.x - midPos.x;
	startPosLocal.y = start.y - midPos.y;
	startPosLocal.z = start.z - midPos.z;
	endPosLocal.x = end.x - midPos.x;
	endPosLocal.y = end.y - midPos.y;
	endPosLocal.z = end.z - midPos.z;

	//Y軸回転
	float angleY = atan2f(endPosLocal.x - startPosLocal.x, endPosLocal.z - startPosLocal.z);
	m_rotation.y = XMConvertToDegrees(angleY);
}

//壁との衝突収集
void LineBB::CollectWallCollisions()
{
	//衝突点配列クリア
	m_wallCollisionPoints.clear();

	//衝突情報を走査
	const auto& collisionInfos = m_pCollider->GetCollisionInfos();
	for (const auto& info : collisionInfos)
	{
		if (info.state == COLLISION_EXIT) continue;	//衝突終了の場合はスキップ

		//衝突相手のタグを取得
		OBJECT_TAG opponentTag = info.opponent->GetOwner()->GetTag();

		//壁との衝突のみ収集
		if (opponentTag == OBJECT_TAG::WALL)
		{
			m_wallCollisionPoints.push_back(info.contactPoint);
		}
	}
}
