#pragma once
#include "ObjectBase.h"
#include <DirectXMath.h>

class TransformItem : public ObjectBase
{
public:
    TransformItem(DirectX::XMFLOAT3 pos);

protected:
    void UpdateOverride() override;
    void ResolveCollisionsOverride() override;

private:
    float m_livedTime = 0.0f;      // oŒ»‚µ‚Ä‚©‚ç‚ÌŒo‰ßŠÔ
    const float m_maxLifeTime = 20.0f; // ©“®Á–Å‚Ü‚Å‚ÌŠÔ

};
