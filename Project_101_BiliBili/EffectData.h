#pragma once
#include <DirectXMath.h>
#include "RenderData.h"

//エフェクトタイプ列挙体
enum class EFFECT_TYPE
{
	NONE = 0,		//なし
	PARTICLE_POINT,	//パーティクルポイント
	PARTICLE_SPREAD,//パーティクルスプレッド
	MAX				//最大数
};

//エフェクト呼び出し構造体
struct EffectCommand
{
	EFFECT_TYPE type = EFFECT_TYPE::NONE;	//エフェクトタイプ
	DirectX::XMFLOAT3 position{};			//座標
	DirectX::XMFLOAT2 size{};				//サイズ
};

//エフェクトテンプレート構造体
struct EffectTemplate
{
	//描画関連
	EFFECT_TYPE type = EFFECT_TYPE::NONE;	//エフェクトタイプ
	MESH_TYPE meshType{};			//メッシュデータ
	const wchar_t* texPath = L"";			//テクスチャパス
	TexSplitInfo texSplitInfo{};			//テクスチャ分割情報
	BLEND_MODE blendMode = 
		BLEND_MODE::BLEND_TRANSPARENT;		//ブレンドモード

	//オブジェクト関連
	DirectX::XMFLOAT2 baseSize{};			//基本サイズ
	DirectX::XMFLOAT4 baseColor{};			//基本色RGBA
	float lifeTime = 0.0f;					//寿命
};

//シーン別エフェクトテンプレートリスト
extern const EffectTemplate g_effectTemplateList[];

//エフェクトテンプレートセット構造体
struct EffectTemplateSet
{
	EffectTemplate* pTemplates = nullptr;	//エフェクトテンプレートリストポインタ
	int templateCount = 0;					//エフェクトテンプレート数
};

//エフェクトテンプレートセット取得関数
EffectTemplateSet GetEffectTemplate();