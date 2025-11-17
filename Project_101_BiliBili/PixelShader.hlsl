//================================================================================================
//ピクセルシェーダー
//頂点シェーダーから送られてきた座標情報を受け取り、レンダーターゲットへ書き込むための色を返す。
//================================================================================================
#include "BasicShader.hlsli"
//定数バッファ０
cbuffer Transform : register(b0)
{
    float4x4 world;     //ワールド行列
    float4x4 view;      //ビュー行列
    float4x4 proj;      //プロジェクション行列
    float4 objColor;   //全体の色
}

Texture2D gTexture : register(t0);      //テクスチャオブジェクト
SamplerState gSampler : register(s0);   //サンプラーオブジェクト

float4 BasicPS(
    VSOutPut input //頂点シェーダーから送られてきたデータ構造体
) : SV_TARGET //レンダーターゲットへ出力
{
    float4 texColor = gTexture.Sample(gSampler, input.uv); //テクスチャの色を取得

    return texColor * input.color * objColor; //頂点カラーをそのまま返す
}

float4 BasicPSMasked(
    VSOutPut input
) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.uv);
    
    //アルファテスト
    clip(texColor.a - 0.5f);
    return texColor * input.color;

}