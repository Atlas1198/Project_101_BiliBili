#include "LoadUI.h"

LoadUI::LoadUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
}

void LoadUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//背景画像UI生成
	m_pBackgroundImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 1920.0f, 1080.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
		0,												//描画順序
		L"asset/texture/loading_scene/LOAD_SCENE.png",
		PSO_KEY_TRANSPARENT								//ブレンドモード
	);
}

void LoadUI::UpdateOverride()
{
}

void LoadUI::FinalizeOverride()
{
}
