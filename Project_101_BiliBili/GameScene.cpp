#include "GameScene.h"
#include "TitleScene.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

using namespace RenderData;

//コンストラクタ
GameScene::GameScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pPlayerManager = new PlayerManager();	//プレイヤー管理クラスの生成
	m_pFieldManager = new FieldManager();	//フィールド管理クラスの生成
	m_pGameUIManager = new GameUIManager();	//ゲームUI管理クラスの生成
}

//デストラクタ
GameScene::~GameScene()
{
	if (m_pPlayerManager)
	{
		delete m_pPlayerManager;	//プレイヤー管理クラスの削除
		m_pPlayerManager = nullptr;
	}
	if (m_pFieldManager)
	{
		delete m_pFieldManager;		//フィールド管理クラスの削除
		m_pFieldManager = nullptr;
	}
	if (m_pGameUIManager)
	{
		delete m_pGameUIManager;	//ゲームUI管理クラスの削除
		m_pGameUIManager = nullptr;
	}
}

//初期化
void GameScene::InitializeOverride(
	InputManager* pInputManager,		//入力マネージャーのポインタ
	TextureManager& pTextureManager,	//テクスチャ管理クラスのポインタ
	MeshManager& pMeshManager			//メッシュ管理クラスのポインタ
)
{
	m_pPlayerManager->Initialize(	//プレイヤー管理クラス初期化
		pInputManager,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	m_pFieldManager->Initialize(	//フィールド管理クラス初期化
		pInputManager,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	m_pGameUIManager->Initialize(	//ゲームUI管理クラス初期化
		pTextureManager,
		pMeshManager
	);
}

void GameScene::AddPlayer(uint32_t id, InputManager* pInputManager)
{
	m_pPlayerManager->AddPlayer(id, pInputManager, *m_pCollisionManager);
}

void GameScene::RemovePlayer(uint32_t id)
{
	m_pPlayerManager->RemovePlayer(id);
}

//更新
void GameScene::UpdateOverride()
{
	m_pPlayerManager->Update();	//プレイヤー管理クラス更新
	m_pFieldManager->Update();	//フィールド管理クラス更新
	m_pGameUIManager->Update();	//ゲームUI管理クラス更新

	if (m_pInputManager->GetInputInfo()->enter.trigger)
	{
		if (m_pCamera->GetCameraInfo()->position.y == 0.0f)
		{
			m_pCamera->SetPosition({ 0.0f, 10.0f, -1.0f });
		}
		else
		{
			m_pCamera->SetPosition({ 0.0f, 0.0f, -10.0f });
		}
	}
}

//衝突後処理
void GameScene::ResolveCollisions()
{
	m_pPlayerManager->ResolveCollisions();	//プレイヤー管理クラス衝突後処理
	m_pFieldManager->ResolveCollisions();
}

//描画
void GameScene::DrawOverride(Renderer& pRenderer)
{
	m_pPlayerManager->SubmitDraws(pRenderer);
	m_pFieldManager->SubmitDraws(pRenderer);
	m_pGameUIManager->SubmitDraws(pRenderer);
}

//終了
void GameScene::FinalizeOverride()
{
	m_pPlayerManager->Finalize();	//プレイヤー管理クラス終了
	m_pFieldManager->Finalize();	//フィールド管理クラス終了
	m_pGameUIManager->Finalize();	//ゲームUI管理クラス終了
}