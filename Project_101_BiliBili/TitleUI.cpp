#include "TitleUI.h"

//コンストラクタ
TitleUI::TitleUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
}

//初期化
void TitleUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	m_pBaseImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		TitleUI::BASE_TEXTURE_PATH
	);

	m_pLogImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.2f, 0.0f },
		DirectX::XMFLOAT3{ 0.5f, 0.5f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 1,
		TitleUI::TITLE_LOG_TEXTURE_PATH
	);

	m_pPromptImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, -0.2f, 0.0f },
		DirectX::XMFLOAT3{ 0.48f, 0.125f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 2,
		TitleUI::START_PROMPT_TEXTURE_PATH
	);

}

//更新
void TitleUI::UpdateOverride()
{
}

//終了
void TitleUI::FinalizeOverride()
{
}


void TitleUI::PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
	TextureManager& textureManager,	//テクスチャ管理クラスの参照
	MeshManager& meshManager		//メッシュ管理クラスの参照
)
{
}