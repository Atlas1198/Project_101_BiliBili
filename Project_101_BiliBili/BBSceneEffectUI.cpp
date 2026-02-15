#include "BBSceneEffectUI.h"

BBSceneEffectUI::BBSceneEffectUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order, const std::wstring& texturePath, PSOKey psoKey)
	: UIBase(position, scale, rotation, order, psoKey)
{
}

void BBSceneEffectUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	PSOKey key = PSO_KEY_TRANSPARENT;
	
	m_pTopImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0,0,0 }, 
		DirectX::XMFLOAT3{ 1,1,1 },
		DirectX::XMFLOAT3{ 0,0,0 }, 
		0, 
		L"Resources/Texture/BBSceneEffect_Top.png", 
		PSO_KEY_TRANSPARENT
	);
	m_pBottomImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0,0,0 }, 
		DirectX::XMFLOAT3{ 1,1,1 }, 
		DirectX::XMFLOAT3{ 0,0,0 }, 
		0, 
		L"Resources/Texture/BBSceneEffect_Bottom.png", 
		PSO_KEY_TRANSPARENT
	);
}

void BBSceneEffectUI::UpdateOverride()
{
}

void BBSceneEffectUI::FinalizeOverride()
{
}

void BBSceneEffectUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}