#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "ObjectBase.h"

// スプリング（バネ）クラス
class Spring : public ObjectBase
{
public:
	Spring(
		MESH_TYPE meshType,                      // メッシュタイプ
		DirectX::XMFLOAT3 position,              // 座標
		DirectX::XMFLOAT3 rotation,              // 回転
		DirectX::XMFLOAT3 scale,                 // スケール
		DirectX::XMFLOAT3 velocity,              // 移動速度
		DirectX::XMFLOAT3 launchTarget =         // プレイヤーを飛ばしたい位置（ワールド座標）
		DirectX::XMFLOAT3(0.0f, -4.0f, 5.0f),
		bool isActive = true,                    // アクティブフラグ
		ColliderType colliderType =              // コライダータイプ
		ColliderType::BOX,
		DirectX::XMFLOAT3 collisionBoxSize =     // コライダーのボックスサイズ
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		bool collisionIsTrigger = true           // コライダーのトリガーフラグ
	);
	~Spring() {};

	void UpdateOverride() override;             // 更新
	void ResolveCollisionsOverride() override;  // 衝突解決

	DirectX::XMFLOAT3 GetLaunchTarget() const { return m_launchTarget; }
	void SetLaunchTarget(const DirectX::XMFLOAT3& target) { m_launchTarget = target; }

private:
	DirectX::XMFLOAT3 m_launchTarget{ 0.0f, -4.0f, 5.0f }; // 発射ターゲット
};
