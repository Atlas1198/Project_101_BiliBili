#include "BulletCountUI.h"

//コンストラクタ
BulletCountUI::BulletCountUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{

}

void BulletCountUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//弾数画像UIの作成
	for(int i = 0; i < m_bulletCountMax; ++i)
	{
		m_pBulletImage = AddChild<UIImage>(
			DirectX::XMFLOAT3{ 1.0f - 0.16f * i, i * -0.02f, 0.5f },	//位置
			DirectX::XMFLOAT3{ 0.13f, 0.13f, 1.0f },					//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },						//回転
			m_order + 1,												//描画順序
			BulletCountUI::BULLET_TEXTURE_PATH							//テクスチャパス
		);
	}
}

void BulletCountUI::UpdateOverride()
{
	m_frameCount++;
	if(m_frameCount % 60 == 0) // 60フレームごとに弾を消費(仮)
	{
		m_currentBulletCount--;
		if(m_currentBulletCount < 0)
		{
			m_currentBulletCount = m_bulletCountMax;
		}
	}

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
}

void BulletCountUI::FinalizeOverride()
{
}

void BulletCountUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}
