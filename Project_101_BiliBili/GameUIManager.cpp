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
		DirectX::XMFLOAT3{ -600.0f, 450.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 200.0f, 30.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0											//描画順序
	));
	m_pHPBarUI2 = (new HPBarUI(
		DirectX::XMFLOAT3{ 600.0f, 450.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ -200.0f, 30.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
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

	m_roots.push_back(std::unique_ptr<UIBase>(m_pHPBarUI1));	//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pHPBarUI2));	//ルートUIオブジェクト配列に追加

}

//更新
void GameUIManager::UpdateOverride()
{
}

//終了
void GameUIManager::FinalizeOverride()
{
}

void GameUIManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//HPバーUIの描画情報生成
	m_pHPBarUI1->PrepareRenderInfo(textureManager, meshManager);
	m_pHPBarUI2->PrepareRenderInfo(textureManager, meshManager);
}
