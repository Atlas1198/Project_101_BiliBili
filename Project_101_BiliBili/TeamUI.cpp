#include "TeamUI.h"

using namespace DirectX;

//チームUIクラスのコンストラクタ
TeamUI::TeamUI(
	DirectX::XMFLOAT3 position,
	DirectX::XMFLOAT3 scale,
	DirectX::XMFLOAT3 rotation,
	UINT order,
	const wchar_t* frameTexturePath,
	const wchar_t* gageTexturePath,
	const wchar_t* baseTexturePath,
	const wchar_t* alertTexturePath,
	const wchar_t* iconTexturePath,
	DIRECTION offsetDirection
)
	:UIBase(position, scale, rotation, order)
	, m_pHPBarUI(nullptr)
	, m_pIconUI(nullptr)
	, m_pAlertImage(nullptr)
	, m_originalPosition(position)
{
	const XMFLOAT2 hpBarOffsetValue = { 100.0f, 45.0f };	//HPバーのオフセット値
	const XMFLOAT2 iconOffsetValue = { 255.0f, 5.0f };		//アイコンのオフセット値
	int directionFactor = 0;								//方向係数

	//方向による係数設定
	switch (offsetDirection)
	{
	case DIRECTION::LEFT:
		directionFactor = -1;
		break;
	case DIRECTION::RIGHT:
		directionFactor = 1;
		break;
	default:
		break;
	}

	m_pAlertImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ 765.0f, 248.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0,											//描画順序
		alertTexturePath
	);
	m_pAlertImage->SetActive(false);	//最初は非表示

	m_pHPBarUI = AddChild <HPBarUI>(
		DirectX::XMFLOAT3{ hpBarOffsetValue.x * -directionFactor, hpBarOffsetValue.y, 0.0f }, //位置
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f }, //スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, //回転
		0, //描画順序
		frameTexturePath,
		gageTexturePath,
		baseTexturePath,
		offsetDirection
	);

	m_pIconUI = AddChild<IconUI>(
		DirectX::XMFLOAT3{ iconOffsetValue.x * directionFactor, iconOffsetValue.y, 0.0f }, //位置
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f }, //スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f }, //回転
		0, //描画順序
		iconTexturePath
	);
}

//デストラクタ
TeamUI::~TeamUI()
{
}

//初期化
void TeamUI::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
}

//更新
void TeamUI::UpdateOverride()
{
	UpdateHitEffect();
	//ShakeUI();
}

//終了
void TeamUI::FinalizeOverride()
{
}

//オブジェクトの描画情報生成
void TeamUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	m_pHPBarUI->PrepareRenderInfo(textureManager, meshManager);
	m_pIconUI->PrepareRenderInfo(textureManager, meshManager);
	m_pAlertImage->PrepareRenderInfo(textureManager, meshManager);
}

//HP設定関数
void TeamUI::GetDamage(float health)
{
	if (m_pHPBarUI)
	{
		m_pHPBarUI->SetHealth(health);
	}

	m_hitEffectTimer = 0;
	m_isHitEffect = true;
}

//チームキャラクター設定関数
void TeamUI::SetTeamCharacter(int p1, int p2)
{
	if (m_pIconUI)
	{
		m_pIconUI->SetTeamCharacter(p1, p2);
	}
}

//被弾エフェクト更新関数
void TeamUI::UpdateHitEffect()
{
	m_pAlertImage->SetActive(m_isHitEffect);	//被弾エフェクトの表示切り替え

	//被弾エフェクトの更新
	if (m_isHitEffect)
	{
		m_hitEffectTimer++;
		if (m_hitEffectTimer > 20)	//30フレーム経過したら
		{
			m_isHitEffect = false;
		}
	}
}

//UIシェイク関数
void TeamUI::ShakeUI()
{
	auto position = m_local.position;
	
	if (m_isHitEffect)
	{
		//シェイク処理
		float shakeAmount = 14.0f;	//シェイクの強さ
		position.x = m_originalPosition.x + (rand() % (int)(shakeAmount * 2)) - shakeAmount;
		position.y = m_originalPosition.y + (rand() % (int)(shakeAmount * 2)) - shakeAmount;
		m_localPosition = position;
	}
	else
	{
		//元の位置に戻す
		position.x = m_originalPosition.x;
		position.y = m_originalPosition.y;
	}
}
