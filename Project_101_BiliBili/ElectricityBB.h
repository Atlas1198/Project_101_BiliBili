#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "ObjectBase.h"
#include "SharedStruct.h"

//BBの電気クラス
class ElectricityBB : public ObjectBase
{
public:
	ElectricityBB(	//コンストラクタ
		MESH_TYPE meshType,			//メッシュタイプ
		DirectX::XMFLOAT3 position,				//座標
		DirectX::XMFLOAT3 rotation,				//回転
		DirectX::XMFLOAT3 scale,				//スケール
		DirectX::XMFLOAT3 velocity,				//移動速度
		bool isActive = false,					//アクティブフラグ
		ColliderType colliderType =				//コライダータイプ
		ColliderType::BOX,
		DirectX::XMFLOAT3 collisionBoxSize =	//コライダーのボックスサイズ
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		bool collisionIsTrigger = false			//コライダーのトリガーフラグ
	)
		: ObjectBase(
			meshType,
			position,
			rotation,
			scale,
			velocity,
			isActive,
			OBJECT_TAG::BB_ELECTRICITY,
			CollisionData::COLLISION_LAYER::BB_ELECTRICITY
		)
	{
	}
	~ElectricityBB() {}	//デストラクタ

	//メイン処理関数
	void UpdateOverride() override;					//更新
	void ResolveCollisionsOverride() override;		//衝突解決

	//ゲッター
	bool HasHitPlayer() const;	//プレイヤーに当たったかどうか取得
	int GetHitNum() const;		//当たった回数取得

	//セッター
	void SetStartPos(DirectX::XMFLOAT3 position);
	void SetEndPos(DirectX::XMFLOAT3 position);
	void SetTeamId(int id);
	void SetHasHitPlayer(bool hasHit);
	void SetHitNum(int num);

private:
	DirectX::XMFLOAT3 m_startPos = { 0.0f, 0.0f, 0.0f };	//開始地点(プレイヤー)
	DirectX::XMFLOAT3 m_endPos = { 0.0f, 0.0f, 0.0f };		//終了地点(壁orチームメイト)

	int m_teamId = -1;				//チームID
	bool m_hasHitPlayer = false;	//プレイヤーに当たったかどうかフラグ
	int m_hitNum = 0;				//当たった回数(1フレーム内)

private:
	void UpdateGeometry();	//ジオメトリ更新
};