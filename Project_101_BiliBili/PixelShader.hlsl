//================================================================================================
//ピクセルシェーダー
//頂点シェーダーから送られてきた座標情報を受け取り、レンダーターゲットへ書き込むための色を返す。
//================================================================================================
#include "BasicShader.hlsli"
//定数バッファ０
cbuffer Transform : register(b0)
{
    float4x4 world; //ワールド行列
    float4x4 view; //ビュー行列
    float4x4 proj; //プロジェクション行列
    float4 objColor; //全体の色
    float4 uvRect; //uv矩形情報(x:左, y:上, z:右, w:下)
}

//ビルボード用定数バッファ
cbuffer BillboardObject : register(b1)
{
    //カメラデータ
    float4x4 viewProj; //ビュー×プロジェクション行列
    float3 right; //カメラの右方向ベクトル
    float _pad0; //パディング
    float3 up; //カメラの上方向ベクトル
    float _pad1; //パディング

    //ビルボードデータ
    float3 center; //ビルボードの中心座標
    float _pad2; //パディング
    float2 size; //ビルボードのサイズ
    float2 _padSize; //パディング
    float4 colorBil; //ビルボードの色
    float4 uvRectBil; //uv矩形情報(x:左, y:上, z:右, w:下)
};

Texture2D gTexture : register(t0); //テクスチャオブジェクト
SamplerState gSampler : register(s0); //サンプラーオブジェクト

float4 BasicPS(
    VSOutPut input //頂点シェーダーから送られてきたデータ構造体
) : SV_TARGET //レンダーターゲットへ出力
{
    float2 uv = input.uv;
    uv = uvRect.xy + uv * (uvRect.zw - uvRect.xy); //uv矩形情報を適用
    float4 texColor = gTexture.Sample(gSampler, uv); //テクスチャの色を取得
    return texColor * input.color * objColor; //頂点カラーをそのまま返す
}

//アルファマスク用ピクセルシェーダー
float4 BasicPSMasked(
    VSOutPut input
) : SV_TARGET
{
    float2 uv = input.uv;
    uv = uvRect.xy + uv * (uvRect.zw - uvRect.xy); //uv矩形情報を適用
    float4 texColor = gTexture.Sample(gSampler, uv); //テクスチャの色を取得
    //アルファテスト
    clip(texColor.a - 0.5f);
    return texColor * input.color * objColor;
}

//エフェクト用ピクセルシェーダー
float4 EffectPS(
    VSOutPut input
) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.uv);
    
    return texColor * input.color * colorBil;
}

//エフェクト用アルファマスクピクセルシェーダー
float4 EffectPSMasked(
    VSOutPut input
) : SV_TARGET
{
    float4 texColor = gTexture.Sample(gSampler, input.uv);
    
    //アルファテスト
    clip(texColor.a - 0.5f);
    return texColor * input.color * colorBil;
}