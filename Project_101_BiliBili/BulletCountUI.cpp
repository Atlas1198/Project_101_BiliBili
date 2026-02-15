#include "BulletCountUI.h"

//コンストラクタ
BulletCountUI::BulletCountUI(
	DirectX::XMFLOAT3 position, 
	DirectX::XMFLOAT3 scale,
	DirectX::XMFLOAT3 rotation,
	UINT order,
	std::wstring bulletPass,
	std::wstring crossPass
	)
	: UIBase(position, scale, rotation, order),
	m_bulletPath(bulletPass), m_crossPath(crossPass)
{

}

void BulletCountUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	DirectX::XMFLOAT3 scale = { 105.0f, 117.0f, 1.0f };
	const float scaleFactor = 0.3f; // スケール調整用係数
	const DirectX::XMFLOAT3 adjustedScale = { scale.x * scaleFactor, scale.y * scaleFactor, scale.z };
	const float spacing = -10.0f; // 弾画像間のスペース

	//弾数画像UIの作成
	for(int i = 0; i < m_bulletCountMax; ++i)
	{
		const float basePosX = (-(m_bulletCountMax * 0.5f - 1) * (adjustedScale.x + spacing) - adjustedScale.x * 0.5f);
		DirectX::XMFLOAT3 position = { basePosX + (adjustedScale.x + spacing) * i, 0.0f, 0.5f };
		m_pBulletImage = AddChild<UIImage>(
			position,													//位置
			adjustedScale,												//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },						//回転
			m_order + 1,												//描画順序
			m_bulletPath.c_str()
		);
	}
	m_pCrossImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ 168.0f * 0.8f, 59.0f * 0.8f, 0.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		m_order + 2,								//描画順序
		m_crossPath.c_str()
	);
	m_pCrossImage->SetActive(false);

}

void BulletCountUI::UpdateOverride()
{
	for(int i = 0; i < m_bulletCountMax; ++i)
	{
		if( i < m_currentBulletCount )
		{
			m_children[i]->SetActive(true); // 弾がある場合は表示
		}
		else
		{
			m_children[i]->SetActive(false); // 弾がない場合は非表示
		}
	}

	if (m_currentBulletCount <= 0)
	{
		m_pCrossImage->SetActive(true);
	}
	else
	{
		m_pCrossImage->SetActive(false);
	}
}

void BulletCountUI::FinalizeOverride()
{
}

void BulletCountUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}
