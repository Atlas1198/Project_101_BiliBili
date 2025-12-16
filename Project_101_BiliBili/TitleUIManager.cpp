#include "TitleUIManager.h"

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
	m_pTitleUI = (new TitleUI(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 1920.0f, 1080.0f, 0.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0											//描画順序
	));


	if (m_pTitleUI)
	{
		m_pTitleUI->Initialize(textureManager, meshManager);
	}

	m_roots.push_back(std::unique_ptr<UIBase>(m_pTitleUI));

}

//更新
void TitleUIManager::UpdateOverride()
{
}

//終了
void TitleUIManager::FinalizeOverride()
{
}


void TitleUIManager::PrepareRenderInfo(	//オブジェクトの描画情報生成
	TextureManager& textureManager,	//テクスチャ管理クラスの参照
	MeshManager& meshManager		//メッシュ管理クラスの参照
)
{
	m_pTitleUI->PrepareRenderInfo(textureManager, meshManager);
}