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
	const wchar_t* baseTexturePath,
	DIRECTION gageDecrementDirection
)
	: 
	UIBase(position, scale, rotation, order),
	m_frameTexturePath(frameTexturePath), 
	m_gageTexturePath(gageTexturePath),
	m_baseTexturePath(baseTexturePath),
	m_gageDecrementDirection(gageDecrementDirection)
{
}

//初期化
void HPBarUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	DirectX::XMFLOAT3 BASE_SCALE = {};	//左基準スケール
	DirectX::XMFLOAT3 GAUGE_SCALE = {};	//ゲージスケール
	DirectX::XMFLOAT3 FRAME_SCALE = {};	//フレームスケール
	if(m_gageDecrementDirection == DIRECTION::LEFT)
	{
		BASE_SCALE = { 488.0f, 95.0f, 1.0f };
		GAUGE_SCALE = { 468.0f, 76.0f, 1.0f };
		FRAME_SCALE = { 488.0f, 76.0f, 1.0f };
	}
	else
	{
		BASE_SCALE = { 487.0f, 94.0f, 1.0f };
		GAUGE_SCALE = { 368.0f, 76.0f, 1.0f };
		FRAME_SCALE = { 487.0f, 76.0f, 1.0f };
	}

	//背景画像UIの作成
	m_pBaseImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		BASE_SCALE,
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		m_baseTexturePath
	);
	//バー画像UIの作成
	m_pGageImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		GAUGE_SCALE,
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order + 1,
		m_gageTexturePath
	);
	//フレーム画像UIの作成
	m_pFrameImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		FRAME_SCALE,
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
	const int directionFactor = (m_gageDecrementDirection == DIRECTION::LEFT) ? -1 : 1;	//方向係数

	//ローカル変換情報更新
	auto scale = m_pGageImage->GetLocalScale();			//ローカルスケール取得
	scale.x = gageScaleX * m_hpRate;					//スケールX更新
	m_pGageImage->SetLocalScale(scale);
	auto position = m_pGageImage->GetLocalPosition();	//ローカル位置取得
	position.x = directionFactor * (gageScaleX - scale.x) * 0.5f;		//位置X更新
	m_pGageImage->SetLocalPosition(position);			//ローカル変換情報設定

	//UV矩形更新
	UVRect uvRect{};
	uvRect.u = 0.0f;
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
