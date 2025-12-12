#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "ObjectBase.h"
#include <cmath>

// 壁クラス
class Wall : public ObjectBase
{
public:
    Wall(    // コンストラクタ(引数あり)
        MESH_TYPE meshType,           // メッシュタイプ
        DirectX::XMFLOAT3 position,             // 座標
        DirectX::XMFLOAT3 rotation,             // 回転（度）
        DirectX::XMFLOAT3 scale,                // スケール
        DirectX::XMFLOAT3 velocity,             // 移動速度
        bool isActive = true,                   // アクティブフラグ
        ColliderType colliderType =             // コライダータイプ
        ColliderType::BOX,
        DirectX::XMFLOAT3 collisionBoxSize =    // コライダーのボックスサイズ
        DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
        bool collisionIsTrigger = false,        // コライダーのトリガーフラグ
        float rotationSpeed = 0.0f,             // 回転速度（度／フレーム）
        bool orbit = false,                     // orbit: trueなら中心周りを回る（位置が動く）
        float orbitRadius = 0.0f,               // orbit の半径（ワールド単位）。0なら初期位置から計算
        DirectX::XMFLOAT3 orbitCenter = 
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f)     // 回転中心（ワールド座標）
    );
    ~Wall() {};    // デストラクタ

    void UpdateOverride() override;                // 更新
    void ResolveCollisionsOverride() override;     // 衝突解決

private:
    float m_rotationSpeed = 0.0f;                 // 回転速度（度／フレーム）
    bool m_orbit = false;                         // orbit するかどうか
    float m_orbitRadius = 0.0f;                   // orbit 半径
    DirectX::XMFLOAT3 m_orbitCenter = 
        DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);      // orbit 中心
    float m_currentOrbitAngleDeg = 0.0f;          // 現在の orbit 角度（度）
};
