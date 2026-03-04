#include "BBGageUI.h"

BBGageUI::BBGageUI(
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 scale, 
	DirectX::XMFLOAT3 rotation, 
	UINT order, 
	DIRECTION offsetDirection,
	const wchar_t* gageTexturePath
)
	: UIBase(position, scale, rotation, order), m_gageDecrementDirection(offsetDirection)
{
	m_pGageImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 50.0f, 0.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		order,
		gageTexturePath,
		PSO_KEY_TRANSPARENT
	);

	m_pGageImage->SetTexSplitInfo({
		0,
		8,
		4,
		30,
		0,
		3
		});
}

void BBGageUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

void BBGageUI::UpdateOverride()
{
	const float gageScaleY = 468.0f;	//ゲージ画像スケールY基準値

	//ローカル変換情報更新
	auto scale = m_pGageImage->GetLocalScale();			//ローカルスケール取得
	scale.y = gageScaleY * m_pTimeRate;					//スケールy更新
	m_pGageImage->SetLocalScale(scale);
	auto position = m_pGageImage->GetLocalPosition();	//ローカル位置取得
	position.y = (gageScaleY - scale.y) * 0.5f;		//位置y更新
	m_pGageImage->SetLocalPosition(position);			//ローカル変換情報設定

	//UV矩形更新
	UVRect uvRect{};
	uvRect.u = 0.0f;
	uvRect.v = 1.0f - m_pTimeRate;
	uvRect.su = 1.0f;
	uvRect.sv = m_pTimeRate;
	m_pGageImage->SetUVRect(uvRect);
}

void BBGageUI::FinalizeOverride()
{
}

void BBGageUI::SetGage(float ratio)
{
	// ゲージの割合を0.0fから1.0fの範囲でクランプ
	m_pTimeRate = std::max(0.0f, std::min(1.0f, ratio));
}
