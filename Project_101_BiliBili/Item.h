#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "ObjectBase.h"

//壁クラス
class Item : public ObjectBase
{
public:
	Item(	//コンストラクタ(引数あり)
		MESH_TYPE meshType,			//メッシュタイプ
		DirectX::XMFLOAT3 position,				//座標
		DirectX::XMFLOAT3 rotation,				//回転
		DirectX::XMFLOAT3 scale,				//スケール
		DirectX::XMFLOAT3 velocity,				//移動速度
		bool isActive = true,					//アクティブフラグ
		ColliderType colliderType =				//コライダータイプ
		ColliderType::BOX,
		DirectX::XMFLOAT3 collisionBoxSize =	//コライダーのボックスサイズ
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		bool collisionIsTrigger = false			//コライダーのトリガーフラグ
	);
	~Item() {};	//デストラクタ

	void UpdateOverride() override;				//更新
	void ResolveCollisionsOverride() override;	//衝突解決

private:
};