#include "ResultUI.h"

//コンストラクタ
ResultUI::ResultUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
	//背景画像UI生成
	m_pBackgroundImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//位置
		DirectX::XMFLOAT3{ 1920.0f, 1080.0f, 1.0f },		//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Back.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);
	m_pBackgroundImage->SetActive(false); //非アクティブに設定
}

//初期化
void ResultUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//更新
void ResultUI::UpdateOverride()
{
	m_mainTimer++;	//メインタイマー更新
}

//終了
void ResultUI::FinalizeOverride()
{
}

void ResultUI::ShowResult()
{
	m_pBackgroundImage->SetActive(true); //背景画像をアクティブに設定
	m_mainTimer = 0; //メインタイマーリセット
}

//オブジェクトの描画情報生成
void ResultUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	m_pBackgroundImage->PrepareRenderInfo(textureManager, meshManager);
}
