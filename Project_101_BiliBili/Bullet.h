#pragma once
#include "ObjectBase.h"
#include <DirectXMath.h>

class Bullet : public ObjectBase
{
public:
    Bullet(
        const DirectX::XMFLOAT3& pos,
        const DirectX::XMFLOAT3& dir,
        float speed,
        int ownerTeam,
		uint32_t ownerID,
        float lifeTimeSec = 3.0f,  //存在時間
        float maxDistance = 50.0f  //最大移動距離
    );
    void UpdateOverride() override;
    void ResolveCollisionsOverride() override;
    bool DeleteFlag() const { return m_deleteFlag; }    //Bullet専用の生存管理
    DirectX::XMFLOAT3 GetPosition() const { return m_position; }    //座標を返す

private:
    DirectX::XMFLOAT3 m_direction;  //移動方向
    float m_speed;                  //移動速度
    int   m_ownerTeam;              //発射したチーム（味方・敵判定用）
	uint32_t m_ownerID;            //発射したプレイヤーのID
    float m_lifeTime;               //最大存在時間
    float m_livedTime = 0.0f;       //現在の生存時間
    float m_maxDistance;            //最大移動距離
    float m_traveled = 0.0f;        //移動した距離
    bool m_deleteFlag = false;
};