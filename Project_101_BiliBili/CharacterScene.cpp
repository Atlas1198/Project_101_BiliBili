#include "CharacterScene.h"

//コンストラクタ
CharacterScene::CharacterScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pCharacterUIManager = new CharacterUIManager(window_width, window_height);
}

//デストラクタ
CharacterScene::~CharacterScene()
{
	if (m_pCharacterUIManager)
	{
		delete m_pCharacterUIManager;
		m_pCharacterUIManager = nullptr;
	}
}

//シーン固有の初期化
void CharacterScene::InitializeOverride(
	InputManager* pInputManager,		//入力管理クラスのポインタ
	TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
	MeshManager& pMeshManager			//メッシュ管理クラスの参照
)
{
	m_pCharacterUIManager->Initialize(pTextureManager, pMeshManager);
}

//シーン固有の更新
void CharacterScene::UpdateOverride()
{
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
	m_pCharacterUIManager->Finalize();
}
