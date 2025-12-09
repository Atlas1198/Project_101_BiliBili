#include "HPBarUI.h"
#include "EventManager.h"

//コンストラクタ
HPBarUI::HPBarUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
}

//初期化
void HPBarUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//背景画像UIの作成
	m_pBaseImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		HPBarUI::BASE_TEXTURE_PATH
	);
	//バー画像UIの作成
	m_pGageImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 1,
		HPBarUI::GAGE_TEXTURE_PATH
	);
	//フレーム画像UIの作成
	m_pFrameImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 1.02f, 1.02f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 2,
		HPBarUI::FRAME_TEXTURE_PATH
	);

}

//更新
void HPBarUI::UpdateOverride()
{
	UpdateGageImage();
}

//終了
void HPBarUI::FinalizeOverride()
{
}

//オブジェクトの描画情報生成
void HPBarUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//ゲージ画像更新関数
void HPBarUI::UpdateGageImage()
{
	auto local = m_pGageImage->GetLocalTransform();
	local.scale.x = 1.0f * m_hpRate;
	local.position.x = -(1.0f - local.scale.x) * 0.5f;
	m_pGageImage->SetLocalTransform(local);

	UVRect uvRect{};
	uvRect.u = 0.0;
	uvRect.v = 0.0f;
	uvRect.su = m_hpRate;
	uvRect.sv = 1.0f;

	m_pGageImage->SetUVRect(uvRect);
}

void HPBarUI::SetHealth(float health)
{
	m_hpRate = (std::max)(0.0f, health);
}
