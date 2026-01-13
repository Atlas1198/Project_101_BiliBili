#pragma once
#include "ObjectBase.h"
#include <DirectXMath.h>

class BilibiliArea : public ObjectBase
{
public:
    BilibiliArea(
        const DirectX::XMFLOAT3 &pos,
        int ownerTeam,
        float damage
    );
    void UpdateOverride() override;
    void ResolveCollisionsOverride() override;
	void SetPlayerPos(const DirectX::XMFLOAT3 &position); //プレイヤー位置の設定
    void SetBB(int teamID, bool active); //BBの有効・無効設定

private:
    int   m_ownerTeam;              //発射したチーム（味方・敵判定用）
    float m_damage;                 //ダメージ量
};