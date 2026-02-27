#pragma once
#include "ContentRoot.h"

inline bool CheckRequiredFiles(const ContentRoot& cr)
{
    // ここは “最低限の必須” だけ入れるのがコツ（増やしすぎない）
    const fs::path mustHave[] = {
        cr.ResolveAsset(L"shader/VertexShader.hlsl"),
        cr.ResolveAsset(L"shader/PixelShader.hlsl"),
        // 必須テクスチャや設定があればここに
        // cr.ResolveAsset(L"texture/default.png"),
    };

    bool ok = true;
    for (const auto& p : mustHave)
    {
        if (!fs::exists(p))
        {
            DebugLogA("[Startup][FATAL] Missing: %ws\n", p.c_str());
            ok = false;
        }
    }
    return ok;
}