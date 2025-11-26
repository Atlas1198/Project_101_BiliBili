#include "HPBarUI.h"

//コンストラクタ
HPBarUI::HPBarUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
}

//初期化
void HPBarUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//背景画像UIの作成
	m_pBgImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 2.0f, 2.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		HPBarUI::BG_TEXTURE_PATH
	);
	//フレーム画像UIの作成
	m_pFrameImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.8f, 1.8f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 1,
		HPBarUI::FRAME_TEXTURE_PATH
	);
	//バー画像UIの作成
	m_pBarImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ -0.6f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 0.7f, 1.8f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 2,
		HPBarUI::BAR_TEXTURE_PATH
	);
}

//更新
void HPBarUI::UpdateOverride()
{
	m_pBgImage->SetColor(DirectX::XMFLOAT4{ 0.0f, 0.0f, 0.0f, 1.0f });
	m_pFrameImage->SetColor(DirectX::XMFLOAT4{ 1.0f, 0.0f, 0.0f, 1.0f });
	m_pBarImage->SetColor(DirectX::XMFLOAT4{ 0.0f, 1.0f, 0.0f, 1.0f });
}

//終了
void HPBarUI::FinalizeOverride()
{
}

//オブジェクトの描画情報生成
void HPBarUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}
