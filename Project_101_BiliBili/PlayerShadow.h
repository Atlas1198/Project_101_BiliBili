#pragma once

#include "ObjectBase.h"

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
		ObjectBase *pTarget,					//影を追従させるオブジェクトへのポインタ
		bool collisionIsTrigger = true					//衝突トリガーフラグ
	);

private:
	ObjectBase* m_pTarget;	//影を追従させるオブジェクトへのポインタ
	DirectX::XMFLOAT3 m_baseShadowScale{};
	void UpdateOverride() override;	//シーン固有の更新
	void ResolveCollisionsOverride() override {}	//衝突解決のオーバーライド（影は衝突解決しないため空実装
};
