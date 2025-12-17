#include "ControllerScene.h"

//コンストラクタ
ControllerScene::ControllerScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pControllerConnecter = new ControllerConnecter();
	m_pUIManager = new ControllerUIManager();
}

//デストラクタ
ControllerScene::~ControllerScene()
{
	delete m_pControllerConnecter;
	delete m_pUIManager;
}

//シーン固有の初期化
void ControllerScene::InitializeOverride(InputManager* pInputManager, TextureManager& pTextureManager, MeshManager& pMeshManager)
{
	m_pControllerConnecter->Initialize();
	m_pUIManager->Initialize(pTextureManager, pMeshManager);
}

//シーン固有の更新
void ControllerScene::UpdateOverride()
{
	m_pControllerConnecter->Update(*m_pInputManager);
	m_pUIManager->Update();

	bool allConnected = m_pControllerConnecter->IsAllConnected();

	if (allConnected)
	{
		//遷移条件の記述
	}
}

//シーン固有の描画
void ControllerScene::DrawOverride(Renderer& pRenderer)
{
	m_pUIManager->SubmitDraws(pRenderer);
}

//シーン固有の終了
void ControllerScene::FinalizeOverride()
{
	m_pUIManager->Finalize();
	m_pControllerConnecter->Finalize();
}
