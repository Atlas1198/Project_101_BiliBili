#include "BBSceneEffectUI.h"

BBSceneEffectUI::BBSceneEffectUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order, const std::wstring& texturePath, PSOKey psoKey)
	: UIBase(position, scale, rotation, order, psoKey)
{
}

void BBSceneEffectUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	PSOKey key = PSO_KEY_TRANSPARENT;
	key.psEntry = PS_ID::BBSceneEffect;
	
	m_pTopImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0,540 - 130 ,0 }, 
		DirectX::XMFLOAT3{ 1980,300,1 },
		DirectX::XMFLOAT3{ 0,0,0 }, 
		0, 
		L"asset/texture/white.png", 
		key
	);
	m_pBottomImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0,-560 + 0, 0 },
		DirectX::XMFLOAT3{ 1980,300,1 },
		DirectX::XMFLOAT3{ 0,0,0 },
		0, 
		L"asset/texture/white.png", 
		key
		);

	m_pTopImage->SetColor({ 0.8f, 0.25f, 1.0f, 1.0f });
	m_pBottomImage->SetColor({ 0.8f, 0.25f, 1.0f, 1.0f });

}

void BBSceneEffectUI::UpdateOverride()
{
	const float dt = 1.0f / 60.0f;

	m_time += dt;
	if (m_time > 1000.0f) m_time -= 1000.0f;

	// UVは固定（スクロールしない）
	UVRect rect = { 0.0f, 0.0f, 1.0f, 1.0f };
	m_pTopImage->SetUVRect(rect);
	m_pBottomImage->SetUVRect(rect);

	// objColor = (intensity, feather, timeSec, alpha)
	m_pTopImage->SetColor({ 1.0f, 0.25f, m_time, 1.0f });
	m_pBottomImage->SetColor({ 1.0f, 0.25f, m_time, 1.0f });
}

void BBSceneEffectUI::FinalizeOverride()
{
}

void BBSceneEffectUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}