#include "CharacterScene.h"
#include "AudioManager.h"

//コンストラクタ
CharacterScene::CharacterScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pCharacterSelecter = new CharacterSelecter();
	m_pCharacterUIManager = new CharacterUIManager(m_pCamera->GetCameraInfo(), window_width, window_height);
}

//デストラクタ
CharacterScene::~CharacterScene()
{
	if (m_pCharacterUIManager)
	{
		delete m_pCharacterUIManager;
		m_pCharacterUIManager = nullptr;
	}
	if (m_pCharacterSelecter)
	{
		delete m_pCharacterSelecter;
		m_pCharacterSelecter = nullptr;
	}
}

//シーン固有の初期化
void CharacterScene::InitializeOverride(
	TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
	MeshManager& pMeshManager			//メッシュ管理クラスの参照
)
{
	m_pCharacterSelecter->Initialize(); 
	m_pCharacterUIManager->Initialize(pTextureManager, pMeshManager);
	m_pCharacterUIManager->StartFadeIn(0.05f);

}

//シーン固有の更新
void CharacterScene::UpdateOverride()
{
	m_pCharacterSelecter->Update(*m_pSceneContext);
	m_pCharacterUIManager->Update();
}

//シーン固有の描画
void CharacterScene::DrawOverride(Renderer& pRenderer)
{
	m_pCharacterUIManager->SubmitDraws(pRenderer);
}

//シーン固有の終了
void CharacterScene::FinalizeOverride()
{
	m_pCharacterSelecter->Finalize(*m_pSceneContext);
	m_pCharacterUIManager->Finalize();
}
