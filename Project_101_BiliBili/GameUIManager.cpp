#include "GameUIManager.h"

//デストラクタ
GameUIManager::~GameUIManager()
{
}

//初期化
void GameUIManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	//HPバーUIの作成
	m_pHPBarUI1 = (new HPBarUI(
		DirectX::XMFLOAT3{ -500.0f, 440.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ -488.0f, 105.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0											//描画順序
	));
	m_pHPBarUI2 = (new HPBarUI(
		DirectX::XMFLOAT3{ 500.0f, 440.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 488.0f, 105.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0											//描画順序
	));

	m_pBulletCountUI1 = (new BulletCountUI(
		DirectX::XMFLOAT3{ -500.0f, 390.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ -220.0f, 209.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0											//描画順序
	));
	m_pBulletCountUI2 = (new BulletCountUI(
		DirectX::XMFLOAT3{ 500.0f, 390.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 220.0f, 209.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0											//描画順序
	));

	m_pIconUI1 = (new IconUI(
		DirectX::XMFLOAT3{ -850.0f, 430.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 130.0f, 130.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 45.0f },		//回転
		0											//描画順序
	));
	m_pIconUI2 = (new IconUI(
		DirectX::XMFLOAT3{ 850.0f, 430.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 130.0f, 130.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 45.0f },		//回転
		0											//描画順序
	));

	//HPバーUIの初期化
	if (m_pHPBarUI1)
	{
		m_pHPBarUI1->Initialize(textureManager, meshManager);
	}
	if (m_pHPBarUI2)
	{
		m_pHPBarUI2->Initialize(textureManager, meshManager);
	}
	//弾数UIの初期化
	if(m_pBulletCountUI1)
	{
		m_pBulletCountUI1->Initialize(textureManager, meshManager);
	}
	if(m_pBulletCountUI2)
	{
		m_pBulletCountUI2->Initialize(textureManager, meshManager);
	}

	//アイコンUIの初期化
	if(m_pIconUI1)
	{
		m_pIconUI1->Initialize(textureManager, meshManager);
	}
	if(m_pIconUI2)
	{
		m_pIconUI2->Initialize(textureManager, meshManager);
	}

	m_roots.push_back(std::unique_ptr<UIBase>(m_pHPBarUI1));	//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pHPBarUI2));	//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBulletCountUI1)); //ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBulletCountUI2)); //ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pIconUI1)); //ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pIconUI2)); //ルートUIオブジェクト配列に追加
}

//更新
void GameUIManager::UpdateOverride()
{
}

//終了
void GameUIManager::FinalizeOverride()
{
}

void GameUIManager::TakeDamage(int teamID, float damage)
{
	if (teamID == 0 && m_pHPBarUI1)
	{
		m_pHPBarUI1->TakeDamage(damage);
	}
	else if (teamID == 1 && m_pHPBarUI2)
	{
		m_pHPBarUI2->TakeDamage(damage);
	}
}

void GameUIManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//HPバーUIの描画情報生成
	m_pHPBarUI1->PrepareRenderInfo(textureManager, meshManager);
	m_pHPBarUI2->PrepareRenderInfo(textureManager, meshManager);
	m_pBulletCountUI1->PrepareRenderInfo(textureManager, meshManager);
	m_pBulletCountUI2->PrepareRenderInfo(textureManager, meshManager);
	m_pIconUI1->PrepareRenderInfo(textureManager, meshManager);
	m_pIconUI2->PrepareRenderInfo(textureManager, meshManager);
}
