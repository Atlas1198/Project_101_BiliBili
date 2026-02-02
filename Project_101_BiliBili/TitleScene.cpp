#include "TitleScene.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "EventManager.h"
#include "SceneManager.h"
#include "AudioManager.h"

//コンストラクタ
TitleScene::TitleScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pTitleUIManager = new TitleUIManager(m_pCamera->GetCameraInfo(), window_width, window_height);
}

//デストラクタ
TitleScene::~TitleScene()
{
	if (m_pTitleUIManager)
	{
		delete m_pTitleUIManager;
		m_pTitleUIManager = nullptr;
	}
}

//初期化
void TitleScene::InitializeOverride(
	InputManager* pInputManager,		//入力管理クラスのポインタ
	TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
	MeshManager& pMeshManager			//メッシュ管理クラスの参照
)
{
	m_pTitleUIManager->Initialize(pTextureManager, pMeshManager);
	AudioManager::GetInstance()->StopBGM();
	//AudioManager::GetInstance()->PlayBGM("TITLE_BGM");
}

//更新
void TitleScene::UpdateOverride()
{
	if (!m_pTitleUIManager->IsFading())
	{
		//スペースキーでタイトルシーンへ遷移(テスト用)
		if (m_pInputManager != nullptr &&
			m_pInputManager->GetInputInfo() != nullptr &&
			m_pInputManager->GetInputInfo()->key.space.trigger)
		{
			//フェードアウト開始
			m_pTitleUIManager->StartFadeOut(0.05f);
			AudioManager::GetInstance()->PlaySE("TITLE_NEXT");
			AudioManager::GetInstance()->StopBGM();
		}
		//コントローラーの任意のボタン入力でコントローラー設定シーンへ遷移
		for (auto& controller : m_pInputManager->GetInputInfo()->controller)
		{
			if (controller.anyButton.trigger)
			{
				//フェードアウト開始
				m_pTitleUIManager->StartFadeOut(0.05f);
				AudioManager::GetInstance()->PlaySE("TITLE_NEXT");
				AudioManager::GetInstance()->StopBGM();
			}
		}
	}

	m_pTitleUIManager->Update();

	if(m_pTitleUIManager->IsFadeEnd())
	{
		//シーン変更イベント発行
		EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
			EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_CONTROLLER);
	}
}

//描画
void TitleScene::DrawOverride(Renderer& pRenderer)
{
	m_pTitleUIManager->SubmitDraws(pRenderer);
}

//終了
void TitleScene::FinalizeOverride()
{
	m_pTitleUIManager->Finalize();
}

//カメラ情報取得
CameraInfo* TitleScene::GetCameraInfo() const
{
	return m_pCamera->GetCameraInfo();
}
