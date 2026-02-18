#pragma once

#include "ObjectBase.h"

class Player;	//前方宣言
//プレイヤーの影クラス
class PlayerShadow : public ObjectBase
{
	public:
	PlayerShadow(	//コンストラクタ
		MESH_TYPE meshType,								//メッシュタイプ
		DirectX::XMFLOAT3 position,						//座標
		DirectX::XMFLOAT3 rotation,						//回転
		DirectX::XMFLOAT3 scale,						//スケール
		DirectX::XMFLOAT3 velocity,						//移動速度
		ColliderType colliderType, //コライダータイプ
		DirectX::XMFLOAT3 collisionBoxSize,
		Player *pPlayer,						//プレイヤーオブジェクトへのポインタ
		bool collisionIsTrigger = true					//衝突トリガーフラグ
	);

private:
	Player* m_pPlayer;	//プレイヤーオブジェクトへのポインタ
	void UpdateOverride() override;	//シーン固有の更新
	void ResolveCollisionsOverride() override {}	//衝突解決のオーバーライド（影は衝突解決しないため空実装
};