#include "EffectData.h"

using namespace DirectX;


//エフェクトテンプレートリスト
const EffectTemplate g_effectTemplateList[] =
{
//パーティクルポイント
    {
         EFFECT_TYPE::PARTICLE_POINT,				//エフェクトタイプ
         MESH_TYPE::SPHERE,					        //メッシュデータ
         L"asset/texture/Particle.png",	//テクスチャパス
         { 1,1 },									//テクスチャ分割情報
         BLEND_MODE::BLEND_TRANSPARENT,				//ブレンドモード
         { 1.0f,1.0f },								//基本サイズ
         { 1.0f,0.0f,0.0f,1.0f },					//基本色RGBA
         30.0f										//寿命
    }
};

//シーン別エフェクトテンプレートリスト
EffectTemplateSet GetEffectTemplate()
{
    EffectTemplateSet set;
    set.pTemplates = const_cast<EffectTemplate*>(g_effectTemplateList);
    set.templateCount = sizeof(g_effectTemplateList) / sizeof(EffectTemplate);
    return set;
}
