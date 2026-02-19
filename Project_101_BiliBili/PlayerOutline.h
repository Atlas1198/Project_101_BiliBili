#pragma once

#include "ObjectBase.h"

class Player;	//前方宣言
//プレイヤーの影クラス
class PlayerOutline : public ObjectBase
{
public:
	PlayerOutline(	//コンストラクタ
		MESH_TYPE meshType,								//メッシュタイプ
		DirectX::XMFLOAT3 position,						//座標
		DirectX::XMFLOAT3 rotation,						//回転
		DirectX::XMFLOAT3 scale,						//スケール
		DirectX::XMFLOAT3 velocity,						//移動速度
		ColliderType colliderType, //コライダータイプ
		DirectX::XMFLOAT3 collisionBoxSize,
		bool collisionIsTrigger = true					//衝突トリガーフラグ
	);

	void SetTexSplitInfo(const TexSplitInfo &info) { m_texSplitInfo = info; } // テクスチャ分割情報構造体セット

private:
	void UpdateOverride() override;	//シーン固有の更新
	void ResolveCollisionsOverride() override {}	//衝突解決のオーバーライド（影は衝突解決しないため空実装
};