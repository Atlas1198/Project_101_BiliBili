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
}

//デストラクタ
GameScene::~GameScene()
{
	delete m_pPlayerManager;	//プレイヤー管理クラスの削除
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
}

//衝突後処理
void GameScene::ResolveCollisions()
{
	m_pPlayerManager->ResolveCollisions();	//プレイヤー管理クラス衝突後処理
}

//描画
void GameScene::DrawOverride(Renderer& pRenderer)
{
	m_pPlayerManager->SubmitDraws(pRenderer);
}

//終了
void GameScene::Finalize()
{
}