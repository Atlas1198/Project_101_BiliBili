#include "SceneManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "EventManager.h"

//コンストラクタ
SceneManager::SceneManager(float windowWidth, float windowHeight)
{
	m_pTitleScene = new TitleScene(windowWidth, windowHeight);				//タイトルシーンクラスの生成
	m_pControllerScene = new ControllerScene(windowWidth, windowHeight);	//コントローラーシーンクラスの生成
	m_pCharacterScene = new CharacterScene(windowWidth, windowHeight);		//キャラクターシーンクラスの生成
	m_pGameScene = new GameScene(windowWidth, windowHeight);				//ゲームシーンクラスの生成

	m_currentScene = SCENE_TYPE::SCENE_TITLE;	//最初のシーンをタイトルシーンに設定
	m_pCurrentScene = m_pTitleScene;	//最初のシーンをタイトルシーンに設定
}

//デストラクタ
SceneManager::~SceneManager()
{
	delete m_pTitleScene;
	delete m_pControllerScene;
	delete m_pCharacterScene;
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

	//シーン変更イベント登録
	using args = SCENE_TYPE;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::CHANGE_SCENE,
		[this](std::shared_ptr<args> data)
		{
			ReserveChangeScene(*data);
		}
	);

	m_pCurrentScene->Initialize(pInputManager,*m_pTextureManager, *m_pMeshManager);
}

//更新
void SceneManager::Update()
{
	m_pCurrentScene->Update();	//現在のシーン更新

	//シーン変更例：タイトルシーンでスペースボタンが押されたらゲームシーンへ変更
	if (m_pCurrentScene == m_pGameScene &&
		m_pInputManager != nullptr &&
		m_pInputManager->GetInputInfo() != nullptr &&
		m_pInputManager->GetInputInfo()->key.space.trigger)
	{
		ReserveChangeScene(SCENE_TYPE::SCENE_TITLE);	//シーン変更
	}

	//シーン変更予約があればシーン変更
	if (m_sceneChangeReserved)
	{
		ChangeScene(m_reservedScene);
	}
}

//終了
void SceneManager::Finalize()
{
}

//シーン変更予約
void SceneManager::ReserveChangeScene(SCENE_TYPE newScene)
{
	m_sceneChangeReserved = true;	//シーン変更予約フラグを立てる
	m_reservedScene = newScene;		//予約されたシーンを保存
}

//シーン変更
void SceneManager::ChangeScene(SCENE_TYPE next)
{
	m_pCurrentScene->Finalize();	//現在のシーン終了処理

	//次のシーンへ変更
	m_sceneChangeReserved = false;	//シーン変更予約フラグを下ろす
	m_reservedScene = SCENE_TYPE::SCENE_NONE;	//予約されたシーンをリセット
	m_currentScene = next;			//現在のシーンを更新

	switch (next)
	{
	case SCENE_TYPE::SCENE_TITLE:
		m_pCurrentScene = m_pTitleScene;
		break;
	case SCENE_TYPE::SCENE_CONTROLLER:
		m_pCurrentScene = m_pControllerScene;
		break;
	case SCENE_TYPE::SCENE_GAME:
		m_pCurrentScene = m_pGameScene;
		break;
	case SCENE_TYPE::SCENE_RESULT:
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

void SceneManager::AddPlayer(uint32_t id)
{
	m_pGameScene->AddPlayer(id, m_pInputManager);
}

void SceneManager::SpawnPlayers()
{
	m_pGameScene->SpawnPlayers(m_pInputManager);
}

void SceneManager::RemovePlayer(uint32_t id)
{
	m_pGameScene->RemovePlayer(id);
}