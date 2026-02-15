//BB有効時の画面エフェクト用ピクセルシェーダー

Texture2D maskTex : register(t0);
SamplerState samp : register(s0);

cbuffer Params : register(b0)
{
    float time;
    float2 uvTiling;
    float2 uvSpeed;
    float intensity;
    float feather;
}

float4 BasicPS(float2 uv : TEXCOORD0) : SV_Target
{
    float2 u = uv * uvTiling + uvSpeed * time;

    float m = maskTex.Sample(samp, u).r;

    float edge = smoothstep(0.0, feather, uv.y) * smoothstep(0.0, feather, 1.0 - uv.y);

    float a = saturate(m * intensity * edge);

    return float4(0, 0, 0, a);
}
