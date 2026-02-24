#include "TitleUIManager.h"

using namespace DirectX;

//デストラクタ
TitleUIManager::~TitleUIManager()
{
}

//初期化
void TitleUIManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	m_pBackImage = new UIImage(
		XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		XMFLOAT3{ m_screenWidth, m_screenWidth / 1421.0f * m_screenHeight, 1.0f },
		XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		0,
		L"asset/texture/title_scene/UI_TITLE_Back.png",
		PSO_KEY_OPAQUE.AddDefines({ SHADER_DEFINE::PS_WRAP_UV })
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBackImage));

	m_pFrameImage = new UIImage(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ m_screenWidth, m_screenHeight, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		1,
		L"asset/texture/title_scene/UI_TITLE_Frame.png"
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pFrameImage));

	m_pButtonImage = new UIImage(
		DirectX::XMFLOAT3{ 0.0f, -300.0f, 0.0f },
		DirectX::XMFLOAT3{ 708.0f, 80.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		2,
		L"asset/texture/title_scene/UI_TITLE_Button.png"
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pButtonImage));

	m_timer = 0;
}

//更新
void TitleUIManager::UpdateOverride()
{
	m_timer++;

	//背景スクロール
	auto uvRect = m_pBackImage->GetUVRect();
	uvRect.u += 0.002f;
	m_pBackImage->SetUVRect(uvRect);

	
	float blinkSpeed = 0.05f; 
	float alpha = sinf(m_timer * blinkSpeed);
	alpha = (alpha + 1.0f) * 0.5f; // 0.0fから1.0fの範囲に変換
	auto color = m_pButtonImage->GetColor();
	color.w = alpha;
	m_pButtonImage->SetColor(color);

}

//終了
void TitleUIManager::FinalizeOverride()
{
}