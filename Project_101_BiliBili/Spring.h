#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include <random>
#include "ObjectBase.h"

class Spring : public ObjectBase
{
public:
    Spring(
        MESH_TYPE meshType,
        DirectX::XMFLOAT3 position,
        DirectX::XMFLOAT3 rotation,
        DirectX::XMFLOAT3 scale,
        DirectX::XMFLOAT3 velocity,
        DirectX::XMFLOAT3 launchTarget,   // ← 今のあなたのFieldManagerに合わせて維持
        bool isActive = true,
        ColliderType colliderType = ColliderType::BOX,
        DirectX::XMFLOAT3 collisionBoxSize = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
        bool collisionIsTrigger = true
    );

    ~Spring() {}

    // 単発（固定）
    void SetLaunchTarget(const DirectX::XMFLOAT3& target);

    // 複数（ランダム可）
    void SetLaunchTargets(const std::vector<DirectX::XMFLOAT3>& targets, bool random = true);

    // Playerが衝突時に呼ぶ（ランダムならここで選ぶ）
    DirectX::XMFLOAT3 ChooseLaunchTarget() const;

    void UpdateOverride() override;
    void ResolveCollisionsOverride() override;
    
     void SetIsBlowing(bool isBlowing) { m_isBlowing = isBlowing; } // Set is blowing flag

private:
    std::vector<DirectX::XMFLOAT3> m_launchTargets;
    bool m_randomLaunch = false;

    // ランダム用（毎回生成しないようにメンバに）
    mutable std::mt19937 m_rng;

	static constexpr double ANIM_TIME = 1.0 / 120.0; // Animation time
	double m_animTime = ANIM_TIME; // Animation time

	bool m_isBlowing = false; // Is blowing flag
	int m_blowCount = 0; // Blow count
};
