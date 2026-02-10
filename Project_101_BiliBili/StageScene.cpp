#include "StageScene.h"
#include "EventManager.h"
#include "EventType.h"
#include "AudioManager.h"

//コンストラクタ
StageScene::StageScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pStageSelector = new StageSelector();
	m_pUIManager = new StageUIManager(&m_pCamera->GetCameraInfo(), window_width, window_height);
}

//デストラクタ
StageScene::~StageScene()
{
	if (m_pStageSelector)
	{
		delete m_pStageSelector;
		m_pStageSelector = nullptr;
	}
}

//シーン固有の初期化
void StageScene::InitializeOverride(
	TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
	MeshManager& pMeshManager			//メッシュ管理クラスの参照
)
{
	m_pStageSelector->Initialize();
	m_pUIManager->Initialize(pTextureManager, pMeshManager);

	using args = STAGE_TYPE;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::CHANGE_STAGE_UI,
		[this](std::shared_ptr<args> data) {
			ChangeStageUI(*data);
		}
	);
	AudioManager::GetInstance()->PlayBGM("CHARA_BGM");
}

//シーン固有の更新
void StageScene::UpdateOverride()
{
	m_pStageSelector->Update(*m_pSceneContext);

	m_pUIManager->Update();

	if (m_pUIManager->IsFadeEnd())
	{
		//ゲームシーンへの遷移イベント発行
		EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
			EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_CHARACTER);

		return;
	}

	//選択完了後処理
	if (!m_pUIManager->IsFading())
	{
		if (m_pStageSelector->IsSelected())
		{
			m_pUIManager->StartFadeOut(0.05f, 60);	//1秒後にフェードアウト開始
			m_pUIManager->StartSelectAnimation();	//ステージ選択アニメーション開始

			//↓↓↓選択時サウンド再生↓↓↓
			AudioManager::GetInstance()->PlaySE("STAGE_SET");
		}
	}
}

//シーン固有の描画
void StageScene::DrawOverride(Renderer& pRenderer)
{
	m_pUIManager->SubmitDraws(pRenderer);
}

//シーン固有の終了
void StageScene::FinalizeOverride()
{
	m_pStageSelector->Finalize();
	m_pUIManager->Finalize();
}

//ステージUI変更関数
void StageScene::ChangeStageUI(STAGE_TYPE stageType)
{
	m_pUIManager->ChangeStageUI(stageType);
}