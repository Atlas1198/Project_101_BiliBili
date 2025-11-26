
#pragma once
#include "Player.h"
#include <vector>

class TransformSystem
{
public:
    void ApplyTransform(Player* player);
    void Update(float deltaTime);

private:
    struct TransformData
    {
        Player* player = nullptr;
        float timer = 10.0f;   // 10•b‚Å‰ğœ
    };

    std::vector<TransformData> m_transformedPlayers;

    void UpdateLaser(TransformData& data);
    bool RayHitEnemy(Player* from, Player* to, Player*& hitEnemy);
};