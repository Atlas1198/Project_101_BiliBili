#include "ControllerScene.h"
#include "AudioManager.h"
//コンストラクタ
ControllerScene::ControllerScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pControllerConnecter = new ControllerConnecter();
	m_pUIManager = new ControllerUIManager(&m_pCamera->GetCameraInfo(), window_width, window_height);
}

//デストラクタ
ControllerScene::~ControllerScene()
{
	delete m_pControllerConnecter;
	delete m_pUIManager;
}

//シーン固有の初期化
void ControllerScene::InitializeOverride(TextureManager& pTextureManager, MeshManager& pMeshManager)
{
	m_pControllerConnecter->Initialize();
	m_pUIManager->Initialize(pTextureManager, pMeshManager, *m_pSceneContext);
	m_pUIManager->StartFadeIn(0.01f);
	AudioManager::GetInstance()->PlayBGM("CON_BGM");
}

//シーン固有の更新
void ControllerScene::UpdateOverride()
{
	m_pControllerConnecter->Update(*m_pSceneContext);
	m_pUIManager->Update();
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
	m_pControllerConnecter->Finalize(*m_pSceneContext);
}