#include "HPBarUI.h"
#include "EventManager.h"

//コンストラクタ
HPBarUI::HPBarUI(
	DirectX::XMFLOAT3 position, 
	DirectX::XMFLOAT3 scale, 
	DirectX::XMFLOAT3 rotation,
	UINT order,
	const wchar_t* frameTexturePath,
	const wchar_t* gageTexturePath,
	const wchar_t* baseTexturePath
)
	: 
	UIBase(position, scale, rotation, order),
	m_frameTexturePath(frameTexturePath), 
	m_gageTexturePath(gageTexturePath),
	m_baseTexturePath(baseTexturePath) 
{
}

//初期化
void HPBarUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//背景画像UIの作成
	m_pBaseImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 488.0f, 95.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		m_baseTexturePath
	);
	//バー画像UIの作成
	m_pGageImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 400.0f, 76.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 1,
		m_gageTexturePath
	);
	//フレーム画像UIの作成
	m_pFrameImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		DirectX::XMFLOAT3{ 488.0f, 76.0f, 1.0f },
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 2,
		m_frameTexturePath
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
	const float gageScaleX = 468.0f;	//ゲージ画像スケールX基準値

	//ローカル変換情報更新
	auto scale = m_pGageImage->GetLocalScale();			//ローカルスケール取得
	scale.x = gageScaleX * m_hpRate;					//スケールX更新
	m_pGageImage->SetLocalScale(scale);
	auto position = m_pGageImage->GetLocalPosition();	//ローカル位置取得
	position.x = -(gageScaleX - scale.x) * 0.5f;		//位置X更新
	m_pGageImage->SetLocalPosition(position);			//ローカル変換情報設定

	//UV矩形更新
	UVRect uvRect{};
	uvRect.u = 0.0;
	uvRect.v = 0.0f;
	uvRect.su = m_hpRate;
	uvRect.sv = 1.0f;
	m_pGageImage->SetUVRect(uvRect);
}

//HP設定関数
void HPBarUI::SetHealth(float health)
{
	m_hpRate = (std::max)(0.0f, health);
}
