#include "GameScene.h"
#include "TitleScene.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "App.h"
#include "EventManager.h"
#include "GameEventManager.h"
#include "AudioManager.h"


using namespace DirectX;

//コンストラクタ
GameScene::GameScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
	m_pPlayerManager = new PlayerManager();	//プレイヤー管理クラスの生成
	m_pFieldManager = new FieldManager();	//フィールド管理クラスの生成
	m_pBulletManager = new BulletManager(); //弾管理クラスの生成
	m_pItemManager = new ItemManager();		//アイテム管理クラスの生成
	m_pBBManager = new BBManager();			//BB管理クラスの生成
	m_pGameEventManager = new GameEventManager(); //イベント管理クラスの生成

	m_pGameUIManager = new GameUIManager(&m_pCamera->GetCameraInfo(), window_width, window_height);	//ゲームUI管理クラスの生成
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
	if (m_pBulletManager)
	{
		delete m_pBulletManager;	//弾管理クラスの削除
		m_pBulletManager = nullptr;
	}
	if (m_pItemManager)
	{
		delete m_pItemManager;		//アイテム管理クラスの削除
		m_pItemManager = nullptr;
	}
	if (m_pBBManager)
	{
		delete m_pBBManager;		//BB管理クラスの削除
		m_pBBManager = nullptr;
	}
	if (m_pGameEventManager)
	{
		delete m_pGameEventManager; //イベント管理クラスの削除
		m_pGameEventManager = nullptr;
	}
}

//初期化
void GameScene::InitializeOverride(
	TextureManager& pTextureManager,	//テクスチャ管理クラスのポインタ
	MeshManager& pMeshManager			//メッシュ管理クラスのポインタ
)
{
	m_begginningBehavior = new BegginningBehavior(this);
	m_countdownBehavior = new CountdownBehavior(this);
	m_playBehavior = new PlayBehavior(this);
	m_resultBehavior = new ResultBehavior(this);

	m_pPlayerManager->Initialize(	//プレイヤー管理クラス初期化
		m_pSceneContext,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	m_pFieldManager->Initialize(	//フィールド管理クラス初期化
		m_pSceneContext,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	m_pGameUIManager->Initialize(	//ゲームUI管理クラス初期化
		pTextureManager,
		pMeshManager,
		*m_pSceneContext
	);

	m_pBulletManager->Initialize( //弾管理クラス初期化
		m_pSceneContext,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	m_pItemManager->Initialize(		//アイテム管理クラス初期化
		m_pSceneContext,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	m_pBulletManager->SetGameUIManager(m_pGameUIManager);

	m_pBBManager->SetGameUIManager(m_pGameUIManager);
	m_pBBManager->SetCollisionManager(m_pCollisionManager);
	m_pBBManager->Initialize(			//BB管理クラス初期化
		m_pSceneContext,
		pTextureManager,
		pMeshManager,
		*m_pCollisionManager
	);

	using args = std::tuple<bool, int, int, int, int>;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::GAME_OVER, 
		[this](std::shared_ptr<args> data)
		{
			SetGameOver(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data));
			if (std::get<4>(*data) >= 0 && std::get<4>(*data) < m_pPlayerManager->GetPlayers().size())
			{
				m_lastDamagedPlayerPosition = m_pPlayerManager->GetPlayers()[std::get<4>(*data)]->GetPosition();
			}
		}
	);

	//タイマーとゲーム状態初期化
	m_timer = 0;								//タイマー初期化
	m_isGameOver = false;						//ゲームオーバーフラグ初期化
	m_currentBehavior = m_begginningBehavior;	//初期状態はカウントダウン前

	m_directionalLight.direction = XMFLOAT3(-0.2f, -1.0f, 0.4f);
	m_directionalLight.intensity = 1.1f;
	m_directionalLight.ambient = 0.1f;

	//カメラ初期化
	m_pCamera->Reset();
	m_pCamera->SetPosition(IN_GAME_CAMERA_POSITION);
	m_pCamera->SetTarget(IN_GAME_CAMERA_TARGET);
	m_pCamera->SetFov(XMConvertToRadians(IN_GAME_CAMERA_FOV));

	m_pGameUIManager->StartFadeIn(0.01f); // ゲームシーンフェードイン
}

void GameScene::AddPlayer(uint32_t id, InputManager* pInputManager)
{
	m_pPlayerManager->AddPlayer(id, pInputManager, *m_pCollisionManager, m_pBulletManager);
}

void GameScene::SpawnPlayers(InputManager* pInputManager)
{
	std::vector<Player *> players;

	for (int i = 0; i < 4; i++)
	{
		PlayerDescription newDesc;
		newDesc.uniqueID = static_cast<uint32_t>(i);
		newDesc.ingameID = static_cast<uint32_t>(i);
		newDesc.pos = {0.0f, 0.0f, 0.0f};
		App::GetInstance()->players.emplace(newDesc.uniqueID, newDesc);

		players.push_back(m_pPlayerManager->AddPlayer(newDesc.uniqueID, pInputManager, *m_pCollisionManager, m_pBulletManager));
	}

	players[0]->SetTeamID(0);
	players[1]->SetTeamID(0);
	players[2]->SetTeamID(1);
	players[3]->SetTeamID(1);

	players[0]->BindTeammate(players[1]);
	players[1]->BindTeammate(players[0]);
	players[2]->BindTeammate(players[3]);
	players[3]->BindTeammate(players[2]);
}

void GameScene::RemovePlayer(uint32_t id)
{
	m_pPlayerManager->RemovePlayer(id);
}

//更新
void GameScene::UpdateOverride()
{
	m_currentBehavior->Update();	//現在のビヘイビアの更新
	m_pPlayerManager->Update();		//プレイヤー管理クラス更新
	m_pFieldManager->Update();		//フィールド管理クラス更新
	m_pBulletManager->Update();		//弾管理クラス更新
	m_pBBManager->SetPlayerData(m_pPlayerManager->GetPlayers());	//プレイヤー位置の設定
	m_pBBManager->Update();		//BB管理クラス更新

	if (m_currentBehavior == m_playBehavior)
	{
		m_pItemManager->Update();		//アイテム管理クラス更新
		m_pGameEventManager->Update();	//イベント管理クラス更新
	}

	m_pGameUIManager->Update();		//ゲームUI管理クラス更新
	m_timer++;						//タイマー更新
}

//衝突後処理
void GameScene::ResolveCollisions()
{
	m_pPlayerManager->ResolveCollisions();	//プレイヤー管理クラス衝突後処理
	m_pFieldManager->ResolveCollisions();
	m_pBulletManager->ResolveCollisions();	//弾管理クラス衝突後処理
	m_pItemManager->ResolveCollisions();	//アイテム管理クラス衝突後処理
	m_pBBManager->ResolveCollisions();		//BB管理クラス衝突後処理
}

//描画
void GameScene::DrawOverride(Renderer& pRenderer)
{
	m_pPlayerManager->SubmitDraws(pRenderer);
	m_pFieldManager->SubmitDraws(pRenderer);
	m_pGameUIManager->SubmitDraws(pRenderer);
	m_pBulletManager->SubmitDraws(pRenderer);
	m_pItemManager->SubmitDraws(pRenderer);
	m_pBBManager->SubmitDraws(pRenderer);
}

//終了
void GameScene::FinalizeOverride()
{
	m_pPlayerManager->Finalize();	//プレイヤー管理クラス終了
	m_pFieldManager->Finalize();	//フィールド管理クラス終了
	m_pGameUIManager->Finalize();	//ゲームUI管理クラス終了
	m_pBulletManager->Finalize();	//弾管理クラス終了
	m_pItemManager->Finalize();		//アイテム管理クラス終了
	m_pBBManager->Finalize();		//BB管理クラス終了
}

void GameScene::ChangeBehavior(GAME_STATE newState)
{
	switch (newState)
	{
	case GAME_STATE::STATE_BEFORE_COUNTDOWN:
		m_currentBehavior = m_begginningBehavior;
		break;
	case GAME_STATE::STATE_COUNTDOWN:
		m_currentBehavior = m_countdownBehavior;
		break;
	case GAME_STATE::STATE_PLAY:
		m_currentBehavior = m_playBehavior;
		break;
	case GAME_STATE::STATE_RESULT:
		m_currentBehavior = m_resultBehavior;
		break;
	default:
		m_currentBehavior = nullptr;
		break;
	}
}

void BegginningBehavior::Update()
{
	const int WAIT_DURATION = 180; // カウントダウン前の待機時間（180フレーム）
	const int BEGIN_DURATION = 40;

	auto eventManager = EventManager::GetInstance();
	auto players = m_pGameScene->m_pPlayerManager->GetPlayers();

	eventManager->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
			players[0]->GetTeamID(), players[0]->GetPosition(), players[1]->GetPosition()
		));
	eventManager->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
			players[2]->GetTeamID(), players[2]->GetPosition(), players[3]->GetPosition()
		));

	if (m_pGameScene->m_timer < WAIT_DURATION)
	{
		if (m_pGameScene->m_timer > BEGIN_DURATION)
		{
			const int ACTIVATE_RATE = 20;

			if (m_pGameScene->m_timer % ACTIVATE_RATE == 0)
			{
				int playerIndex = (m_pGameScene->m_timer - BEGIN_DURATION) / ACTIVATE_RATE - 1;
				eventManager->TriggerEvent<int>(
					EventType::SET_PLAYER_POINTER_ACTIVE, playerIndex
				);
				if (m_pGameScene->m_timer < WAIT_DURATION - BEGIN_DURATION)
				{
					AudioManager::GetInstance()->PlaySE("ANNOUNCE");
				}
			}
		}
	}
	else
	{
		m_pGameScene->ChangeBehavior(GAME_STATE::STATE_COUNTDOWN); // カウントダウン状態に遷移
		m_pGameScene->m_timer = 0; // タイマーリセット
	}
}

void CountdownBehavior::Update()
{
	//定数定義
	const int COUNTDOWN_DURATION = 160;		//カウントダウンの総フレーム数（2.67秒間）
	const int FRAMES_PER_SECOND = 40;		//1秒あたりのフレーム数
	const int START_ANNOUNCE_DURATION = 90; // スタートアナウンスのフレーム数（1.5秒間）

	auto eventManager = EventManager::GetInstance();
	auto players = m_pGameScene->m_pPlayerManager->GetPlayers();

	eventManager->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
			players[0]->GetTeamID(), players[0]->GetPosition(), players[1]->GetPosition()
		));
	eventManager->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
			players[2]->GetTeamID(), players[2]->GetPosition(), players[3]->GetPosition()
		));

	//カウントダウンUIの表示
	if ((m_pGameScene->m_timer % FRAMES_PER_SECOND == 0 && m_pGameScene->m_timer < COUNTDOWN_DURATION))
	{// 1秒ごとにUIを更新
		int second = (COUNTDOWN_DURATION - m_pGameScene->m_timer) / FRAMES_PER_SECOND;
		eventManager->TriggerEvent(EventType::SHOW_COUNT_UI, second);

		AudioManager::GetInstance()->PlaySE("GAME_COUNT1");

		//残り１秒で弾UIを表示
		const int showBulletUISecond = 1;
		if (second == showBulletUISecond)
		{

			//弾UI表示イベントをトリガー
			eventManager->TriggerEvent<std::pair<int, bool>>(
				EventType::SET_BULLET_UI_ACTIVE,
				{ players[0]->GetTeamID(), true }
			);
			eventManager->TriggerEvent<std::pair<int, bool>>(
				EventType::SET_BULLET_UI_ACTIVE,
				{ players[2]->GetTeamID(), true }
			);

			//プレイヤーポインター画像非アクティブ化イベントをトリガー
			eventManager->TriggerEvent(EventType::INACTIVATE_PLAYER_POINTER_IMAGES);
		}
	}

	//カウントダウン終了後、ゲーム状態をプレイに変更
	if (m_pGameScene->m_timer == COUNTDOWN_DURATION) // 4秒カウントダウン
	{
		m_pGameScene->m_pPlayerManager->SetInputAccepted(true);					// プレイヤーの入力を受け付けるようにする
		EventManager::GetInstance()->TriggerEvent(EventType::SHOW_START_UI);
		AudioManager::GetInstance()->PlaySE("GAME_STRAT");//上とセット


		AudioManager::GetInstance()->PlaySE("GAME_COUNT2");
	}

	//スタートアナウンス表示終了後
	if (m_pGameScene->m_timer == COUNTDOWN_DURATION + START_ANNOUNCE_DURATION)
	{
		//スタートアナウンス非表示イベントをトリガー
		EventManager::GetInstance()->TriggerEvent(EventType::HIDE_COUNT_UI);

		//ゴールアナウンス表示イベントをトリガー
		EventManager::GetInstance()->TriggerEvent<bool>(EventType::ANOUNCE_GOAL, true);

		//BGM再生
		AudioManager::GetInstance()->PlayBGM("GAME_BGM");
		m_pGameScene->ChangeBehavior(GAME_STATE::STATE_PLAY); // プレイ状態に遷移
		m_pGameScene->m_pGameEventManager->Start(); // イベントマネージャー開始
		m_pGameScene->m_timer = 0; // タイマーリセット
	}

}

void PlayBehavior::Update()
{
	const int GOAL_ANNOUNCE_DURATION = 120; // ゴールアナウンスのフレーム数（2秒間）

	//ゴールアナウンス表示終了後
	if (m_pGameScene->m_timer == GOAL_ANNOUNCE_DURATION)
	{
		//ゴールアナウンス非表示イベントをトリガー
		EventManager::GetInstance()->TriggerEvent<bool>(EventType::ANOUNCE_GOAL, false);

	}

#ifdef DEBUG
	if (m_pGameScene->m_pSceneContext->pInputInfo->key.enter.trigger)
	{
		if (m_pGameScene->m_pCamera->GetCameraInfo().position.y == 0.0f)
		{
			m_pGameScene->m_pCamera->SetPosition({ 10.0f, 30.0f, -1.0f });
		}
		else
		{
			m_pGameScene->m_pCamera->SetPosition({ 0.0f, 35.0f, -15.0f });
		}
	}
	else
	{
		m_pGameScene->m_pCamera->SetPosition({ 0.0f, 70.0f, -50.0f });
		m_pGameScene->m_pCamera->SetTarget({ 0.0f, 00.0f, 3.5f });
	}
#endif // DEBUG

	//勝利条件の判定
	if (m_pGameScene->m_isGameOver)
	{
		m_pGameScene->m_timer = 0;
		m_pGameScene->ChangeBehavior(GAME_STATE::STATE_RESULT);	// リザルト状態に遷移
		m_pGameScene->m_pPlayerManager->SetInputAccepted(false); // プレイヤーの入力を受け付けないようにする
		XMFLOAT3 cameraDestination = m_pGameScene->m_lastDamagedPlayerPosition;	// カメラ移動目的地の初期値をプレイヤー1の位置に設定

		//カメラ移動演出用のパラメーター設定
		m_pGameScene->m_resultBehavior->SetCameraStartPosition(m_pGameScene->m_pCamera->GetCameraInfo().position);	// カメラ移動開始位置の設定
		m_pGameScene->m_resultBehavior->SetCameraDestination(cameraDestination);									// カメラ移動目的地の設定
		m_pGameScene->m_pCamera->SetTarget(cameraDestination);														// カメラ移動目的地の設定

		//BGM停止、ゲーム終了SE再生
		AudioManager::GetInstance()->StopAll();
		AudioManager::GetInstance()->PlaySE("GAME_FINISH_SHOOT");
		AudioManager::GetInstance()->StopLoopSE("TF_SHOOT");
	}
}

void ResultBehavior::Update()
{
	switch (m_subState)
	{
	case SUB_STATE::CAMERA_MOVE:
		HandleCameraMove();
		break;
	case SUB_STATE::FIRST_WAIT:
		HandleFirstWait();
		break;
	case SUB_STATE::SHOW_RESULT:
		HandleShowResult();
		break;
	case SUB_STATE::THANK_YOU_SCREEN:
		HandleThankYouScreen();
		break;
	}
}

void ResultBehavior::HandleCameraMove()
{
	const float CAMERA_MOVE_DURATION = 60.0f;		// カメラ移動のフレーム数（1秒間）
	const float RESULT_UI_SHOW_TIME = 90.0f;		// リザルトUI表示までのフレーム数（1.5秒間）
	const float THIS_BEHAVIOR_DURATION = 110.0f;	// このビヘイビアの総フレーム数（1.83秒間）
	const float CAMERA_MOVE_MAX_RATE = 0.7f;		// カメラ移動の最大割合(移動開始地点と目的地を結んだ線分上の80%の位置まで移動)

	m_pGameScene->m_pPlayerManager->Update();		//プレイヤー管理クラス更新

	//カメラ位置更新
	float t = m_pGameScene->m_timer / CAMERA_MOVE_DURATION;							// カメラ移動の割合を計算
	t = std::min(t, CAMERA_MOVE_MAX_RATE);											// 移動割合の上限を設定(80%)
	XMFLOAT3 newPosition = LerpXMF3(m_cameraStartPosition, m_cameraDestination, t);	// カメラの新しい位置を線形補間で計算
	m_pGameScene->m_pCamera->SetPosition(newPosition);								// カメラの位置を更新

	//UI表示イベントのトリガー
	if(m_pGameScene->m_timer == RESULT_UI_SHOW_TIME)
	{//カメラ移動が最大割合に達したらリザルトUI表示イベントをトリガー
		AudioManager::GetInstance()->PlaySE("GAME_FINISH");
		EventManager::GetInstance()->TriggerEvent(EventType::SHOW_FINISH_UI);
	}

	//次の状態へ遷移
	if(m_pGameScene->m_timer >= THIS_BEHAVIOR_DURATION)
	{
		m_subState = SUB_STATE::FIRST_WAIT;	// カメラ移動終了後、リザルト表示状態に遷移
		m_pGameScene->m_timer = 0;			// タイマーリセット
	}
}

void ResultBehavior::HandleFirstWait()
{
	const int WAIT_DURATION = 180; // リザルトUI表示までの待機フレーム数（3秒間）
	const int RESULT_BGM_STRAT = 400;	//リザルトBGM再生までの待機フレーム(6秒間)

	//リザルトUI表示前の待機時間中はフィールドとBBを更新し続ける
	if (m_pGameScene->m_timer == 0)
	{
	}

	if (m_pGameScene->m_timer == WAIT_DURATION)
	{//リザルトUI表示前の待機時間終了後、フェードアウト開始
		m_pGameScene->m_pGameUIManager->StartFadeOut(0.02f); // リザルトシーンフェードアウト
	}
	else if (m_pGameScene->m_timer > WAIT_DURATION && m_pGameScene->m_pGameUIManager->IsFadeEnd())
	{//フェードアウト終了後、リザルトUI表示
		m_pGameScene->m_pCamera->SetPosition(GameScene::IN_GAME_CAMERA_POSITION);	// カメラの位置をゲーム中の位置に固定
		m_pGameScene->m_pCamera->SetTarget(GameScene::IN_GAME_CAMERA_TARGET);		// カメラのターゲットをゲーム中のターゲットに固定
		EventManager::GetInstance()->TriggerEvent<std::tuple<int, int, int>>(
			EventType::SHOW_RESULT_UI, {
				m_pGameScene->m_winner,
				m_pGameScene->m_character1ID,
				m_pGameScene->m_character2ID
			}
		);
		EventManager::GetInstance()->TriggerEvent(EventType::HIDE_COUNT_UI);
    AudioManager::GetInstance()->PlaySE("RESULT");
	}
	else if (m_pGameScene->m_timer == RESULT_BGM_STRAT)
	{
		AudioManager::GetInstance()->StopAll();
		AudioManager::GetInstance()->PlayBGM("RESULT_BGM");
		m_subState = SUB_STATE::SHOW_RESULT;
	}
}

void ResultBehavior::HandleShowResult()
{
	//入力情報への参照
	auto& controllers = m_pGameScene->m_pSceneContext->pInputInfo->controller;
	auto& keyInput = m_pGameScene->m_pSceneContext->pInputInfo->key;

	//ボタンが押されているかのフラグ
	bool isPressed = false;

	//スペースキーまたはコントローラーのBボタンが押されているかをチェック
	for (size_t i = 0; i < 4; ++i)
	{
		auto controller = controllers[i];

		if ((controller.B.down || keyInput.space.down) && m_pGameScene->m_pGameUIManager->IsGoToTitleShown())
		{
			isPressed = true;
		}
	}

	//スペースキーまたはコントローラーのBボタンが押されていて、かつタイトルへ戻るUIが表示されている場合
	if (isPressed)
	{
		m_pressTimer++;
		m_pGameScene->m_pGameUIManager->ShakeGoToTitleButton();
		//AudioManager::GetInstance()->PlayBGM("ANNOUNCE");
		
		if (m_pressTimer > PRESS_DURATION)
		{
			m_pGameScene->m_pSceneContext->pInputInfo->SetAllControllerVibration(1.0f, 1.0f, 30);
			m_subState = SUB_STATE::THANK_YOU_SCREEN; // タイトルへ戻るボタンが押された状態に遷移
			m_pressTimer = 0;
			m_pGameScene->m_pGameUIManager->DropThankyouForPlaying(); // プレイしてくれてありがとうUIを表示
			m_pGameScene->m_timer = 0; // タイマーリセット
			AudioManager::GetInstance()->PlaySE("TITLE_NEXT");
		}
	}
	else
	{
		m_pressTimer = std::max(m_pressTimer - 2, 0); //タイマー減少
	}

	m_pGameScene->m_pGameUIManager->SetGoToTitleTimerProgress(	 // タイトルへ戻るUIのタイマーを更新
		static_cast<float>(m_pressTimer) / static_cast<float>(PRESS_DURATION)
	);
}

void ResultBehavior::HandleThankYouScreen()
{
	const int THANK_YOU_DURATION = 180; // エンディング画面表示のフレーム数（3秒間）

	if (m_pGameScene->m_timer == THANK_YOU_DURATION)
	{
		m_pGameScene->m_pGameUIManager->StartFadeOut(0.007f); // リザルトシーンフェードアウト
	}
	else if (m_pGameScene->m_timer > THANK_YOU_DURATION && m_pGameScene->m_pGameUIManager->IsFadeEnd())
	{
		EventManager::GetInstance()->TriggerEvent(EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_TITLE);
	}
}