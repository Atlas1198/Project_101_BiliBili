#include "IconUI.h"

//コンストラクタ
IconUI::IconUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
}

//初期化
void IconUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//アイコン画像UIの作成
		//フレーム画像UIの作成
	m_pFrameImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.1f, 1.1f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 2,
		IconUI::BULLET_TEXTURE_PATH
	);

	//背景画像UIの作成
	m_pBgImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 1,
		IconUI::BULLET_TEXTURE_PATH
	);

	m_pIconImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 0.8f, 0.8f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		IconUI::BULLET_TEXTURE_PATH
	);
}

//更新
void IconUI::UpdateOverride()
{
	m_pIconImage->SetColor(DirectX::XMFLOAT4{ 0.9f, 0.9f, 0.9f, 1.0f });
	m_pBgImage->SetColor(DirectX::XMFLOAT4{ 0.5f, 0.5f, 1.0f, 1.0f });
	m_pFrameImage->SetColor(DirectX::XMFLOAT4{ 0.8f, 0.8f, 0.8f, 1.0f });
}

//終了
void IconUI::FinalizeOverride()
{
}

//オブジェクトの描画情報生成
void IconUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

