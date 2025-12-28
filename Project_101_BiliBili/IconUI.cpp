#include "IconUI.h"

//コンストラクタ
IconUI::IconUI(
	DirectX::XMFLOAT3 position, 
	DirectX::XMFLOAT3 scale, 
	DirectX::XMFLOAT3 rotation, 
	UINT order,
	const wchar_t* texturePath
)
	: UIBase(position, scale, rotation, order), m_texturePath(texturePath)
{
}

//初期化
void IconUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//アイコン画像UIの作成
	m_pIconImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 196.1f, 195.1f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 2,
		m_texturePath,
		BLEND_MODE::BLEND_TRANSPARENT
	);
}

//更新
void IconUI::UpdateOverride()
{
	m_pIconImage->SetColor(DirectX::XMFLOAT4{ 1.0f, 1.0f, 1.0f, 1.0f });
}

//終了
void IconUI::FinalizeOverride()
{
}

//オブジェクトの描画情報生成
void IconUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

