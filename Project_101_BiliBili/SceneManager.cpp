#include "SceneManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

//コンストラクタ
SceneManager::SceneManager(float windowWidth, float windowHeight)
{
	m_pGameScene = new GameScene(windowWidth, windowHeight);	//ゲームシーンクラスの生成
	m_pTitleScene = new TitleScene(windowWidth, windowHeight);	//タイトルシーンクラスの生成

	m_pCurrentScene = m_pTitleScene;	//最初のシーンをゲームシーンに設定
}

//デストラクタ
SceneManager::~SceneManager()
{
	delete m_pGameScene;
}

//初期化
void SceneManager::Initialize(
	InputManager* pInputManager,		//入力管理クラスのポインタ
	TextureManager* pTextureManager,	//テクスチャ管理クラスのポインタ
	MeshManager* pMeshManager			//メッシュ管理クラスのポインタ
)
{
	//メンバ変数に保存
	m_pInputManager = pInputManager;		//入力管理クラスのポインタを保存
	m_pTextureManager = pTextureManager;	//テクスチャ管理クラスのポインタを保存
	m_pMeshManager = pMeshManager;		//メッシュ管理クラスのポインタを保存

	m_pCurrentScene->Initialize(pInputManager,*m_pTextureManager, *m_pMeshManager);
}

//更新
void SceneManager::Update()
{
	m_pCurrentScene->Update();	//現在のシーン更新

	//シーン変更例：タイトルシーンでスペースボタンが押されたらゲームシーンへ変更
	if(m_pCurrentScene == m_pTitleScene && 
	   m_pInputManager != nullptr &&
	   m_pInputManager->GetInputInfo() != nullptr &&
	   m_pInputManager->GetInputInfo()->space.trigger)
	{
		ChangeScene(SCENE_GAME);	//シーン変更
	}
	else if (m_pCurrentScene == m_pGameScene &&
		m_pInputManager != nullptr &&
		m_pInputManager->GetInputInfo() != nullptr &&
		m_pInputManager->GetInputInfo()->space.trigger)
	{
		ChangeScene(SCENE_TITLE);	//シーン変更
	}
}

//終了
void SceneManager::Finalize()
{
}

//シーン変更
void SceneManager::ChangeScene(SCENE next)
{
	m_pCurrentScene->Finalize();	//現在のシーン終了処理

	switch (next)
	{
	case SCENE_TITLE:
		m_pCurrentScene = m_pTitleScene;
		break;
	case SCENE_GAME:
		m_pCurrentScene = m_pGameScene;
		break;
	case SCENE_RESULT:
		//m_pCurrentScene = m_pResultScene;
		break;
	}

	m_pCurrentScene->Initialize(	//新しいシーン初期化
		m_pInputManager,	//入力管理クラスのポインタ
		*m_pTextureManager,	//テクスチャ管理クラスのポインタ
		*m_pMeshManager		//メッシュ管理クラスのポインタ
		); 
}

//描画要求提出
void SceneManager::SubmitDraws(Renderer& pRenderer)
{
	m_pCurrentScene->Draw(pRenderer);
}

//カメラ情報取得
CameraInfo* SceneManager::GetCameraInfo()
{
	return m_pCurrentScene->GetCameraInfo();
}
