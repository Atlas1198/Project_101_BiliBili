#include "BasicShader.hlsli"
//定数バッファ０
cbuffer PerObject : register(b0)
{
    float4x4 world; //ワールド行列
    float4x4 worldInvTranspose; //ワールド行列の逆転置行列
    float4x4 view; //ビュー行列
    float4x4 proj; //プロジェクション行列
    float4 objColor; //全体の色
    float4 uvRect; //uv矩形情報(x:左, y:上, z:右, w:下)
    
    float4 lightDir_Intensity; //ライトの方向(x,y,z)、強度(w)
    float4 lightColor_Ambient; //ライトの色(x,y,z)、環境光強度(w)
}

Texture2D gTexture : register(t0); //テクスチャオブジェクト
SamplerState gSampler : register(s0); //サンプラーオブジェクト

float4 BasicPS(
    VSOutPut input //頂点シェーダーから送られてきたデータ構造体
) : SV_TARGET //レンダーターゲットへ出力
{
    float4 base = gTexture.Sample(gSampler, input.uv) * input.color * objColor;
    
#ifdef PS_USE_MASK
     clip(base.a - 0.1f);
#endif
    
#ifdef PS_MULTIPLY_ALPHA_CONTROL
    base.rgb *= base.a;
#endif

#ifdef PS_USE_LIGHTING
    float3 normal = normalize(input.normal);
    float3 length = normalize(-lightDir_Intensity.xyz);
    float dotValue = saturate(dot(normal, length));
    
    float intensity = lightDir_Intensity.w;
    float3 color = lightColor_Ambient.rgb;
    float ambient = lightColor_Ambient.a;
    
    float3 lit = color * (ambient + dotValue * intensity);
    base = float4(base.rgb * lit, base.a);
#endif
    
#ifdef PS_OUTLINE_RED
    clip(base.a - 0.1f);
    
    return float4(1.0f, 0.188f, 0.188f, 1.0f);
#endif
#ifdef PS_OUTLINE_BLUE
    clip(base.a - 0.1f);
    
    return float4(0.188f, 0.78f, 1.0f, 1.0f);
#endif
    
#ifdef PS_WRAP_UV
    float2 uv = frac(input.uv);
    base = gTexture.Sample(gSampler, uv) * input.color * objColor;
#endif
    
    return base;
}

float4 PostEffectPS(
    VSOutPut input //頂点シェーダーから送られてきたデータ構造体
) : SV_TARGET //レンダーターゲットへ出力
{
    float4 base = gTexture.Sample(gSampler, input.uv);
    
    // Post-process用の処理を追加
    base.rgb = pow(base.rgb, 1.0f / 2.2f); // ガンマ補正
    base.rgb *= 1.5f; // 明るさを上げる

    return base;
}

float Hash21(float2 p)
{
    return frac(sin(dot(p, float2(127.1, 311.7))) * 43758.5453123);
}

float Noise21(float2 p)
{
    float2 i = floor(p);
    float2 f = frac(p);
    float a = Hash21(i);
    float b = Hash21(i + float2(1, 0));
    float c = Hash21(i + float2(0, 1));
    float d = Hash21(i + float2(1, 1));
    float2 u = f * f * (3.0 - 2.0 * f);
    return lerp(lerp(a, b, u.x), lerp(c, d, u.x), u.y);
}

float Stretch1D_T(float x, float t, float w)
{
    float n0 = Noise21(float2(x, t));
    float n1 = Noise21(float2(x + w, t));
    float n2 = Noise21(float2(x - w, t));
    float n3 = Noise21(float2(x + 2.0 * w, t));
    float n4 = Noise21(float2(x - 2.0 * w, t));
    return max(n0, max(max(n1, n2), max(n3, n4)));
}

float4 BBScreenEffectPS(VSOutPut input) : SV_TARGET
{
    float2 uv = input.uv;
    float timeSec = objColor.z;

    // 帯の上下端フェード
    float y = saturate(uv.y);
    float feather = objColor.y;
    float edge = smoothstep(0.0, feather, y) * smoothstep(0.0, feather, 1.0 - y);

    // 走査線単位で位相を変える（帯内に大量に出す）
    float scanCount = 90.0;
    float scan = floor(y * scanCount);
    float phaseX = Hash21(float2(scan, 1.23)) * 13.0;
    float phaseT = Hash21(float2(scan, 9.87)) * 7.0;

    // 横方向の“伸び”を作る（scrollしない：xは固定）
    float density = 50.0; // 密度：上げると細かく、下げると伸びやすい
    float morphSpd = 10.0; // 形の変化速度（スクロールじゃなく“変形”速度）
    float x = uv.x * density + phaseX;
    float t = timeSec * morphSpd + phaseT;

    float base = Stretch1D_T(x * 0.22, t, 0.45); // 0.22↓で長く伸びる、0.45↑で伸び強
    float streak = smoothstep(0.55, 0.90, base);

    // 仕上げのザラつき（点滅に戻らない程度）
    float detail = Noise21(float2(x * 1.6, t * 2.0));
    streak *= lerp(0.75, 1.0, detail);

    // 出現量の揺らぎ（全体が同じにならない）
    float burst = 0.65 + 0.35 * Noise21(float2(timeSec * 1.2, scan * 0.11));

    float intensity = objColor.x;
    float alpha = objColor.w;

    float a = (0.08 + streak * burst * 0.92) * intensity * edge * alpha;
    return float4(0, 0, 0, saturate(a));
}
