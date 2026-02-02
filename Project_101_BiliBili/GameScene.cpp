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

	m_pGameUIManager = new GameUIManager(m_pCamera->GetCameraInfo(), window_width, window_height);	//ゲームUI管理クラスの生成
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
		pMeshManager
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

	using args = std::tuple<bool, int, int, int>;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::GAME_OVER, 
		[this](std::shared_ptr<args> data)
		{
			SetGameOver(std::get<0>(*data), std::get<1>(*data), std::get<2>(*data), std::get<3>(*data));
		}
	);

	//タイマーとゲーム状態初期化
	m_timer = 0;								//タイマー初期化
	m_gameState = GameState::STATE_COUNTDOWN;	//ゲーム状態をカウントダウンに設定
	m_isGameOver = false;						//ゲームオーバーフラグ初期化

	m_directionalLight.direction = XMFLOAT3(-0.2f, -1.0f, 0.4f);
	m_directionalLight.intensity = 1.1f;
	m_directionalLight.ambient = 0.1f;

	m_pCamera->SetPosition({ 0.0f, 70.0f, -50.0f });
	m_pCamera->SetTarget({ 0.0f, 00.0f, 3.5f });
	m_pCamera->SetFov(XMConvertToRadians(20.0f));

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
	if(m_gameState == GameState::STATE_COUNTDOWN)
	{
		//カウントダウン中の処理
		CountdownUpdate();
	}
	else if(m_gameState == GameState::STATE_PLAY)
	{
		//プレイ中の処理
		PlayUpdate();
	}
	else if(m_gameState == GameState::STATE_RESULT)
	{
		//ゲームオーバー時の処理
		ResultUpdate();
	}

	m_pGameUIManager->Update();	//ゲームUI管理クラス更新
}

//衝突後処理
void GameScene::ResolveCollisions()
{
	if(m_gameState != GameState::STATE_PLAY)
	{
		return; // プレイ中でなければ衝突処理を行わない
	}

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

//カウントダウン中の更新処理
void GameScene::CountdownUpdate()
{
	m_pFieldManager->Update();	//フィールド管理クラス更新

	//定数定義
	const int COUNTDOWN_DURATION = 240;	//カウントダウンの総フレーム数（4秒間）
	const int FRAMES_PER_SECOND = 60;	//1秒あたりのフレーム数
	const int COUNTDOWN_START = 30;		//カウントダウン開始フレーム数（1秒間）

	auto eventManager = EventManager::GetInstance();
	auto players = m_pPlayerManager->GetPlayers();


	eventManager->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
			players[0]->GetTeamID(), players[0]->GetPosition(), players[1]->GetPosition()
		));
	eventManager->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
			players[2]->GetTeamID(), players[2]->GetPosition(), players[3]->GetPosition()
		));

	//カウントダウンタイマーの更新
	m_timer++;

	//カウントダウンUIの表示
  	if((COUNTDOWN_DURATION + COUNTDOWN_START - m_timer) % FRAMES_PER_SECOND == 0)
	{// 1秒ごとにUIを更新
		int second = (COUNTDOWN_DURATION + COUNTDOWN_START - m_timer) / FRAMES_PER_SECOND;
		eventManager->TriggerEvent(EventType::SHOW_COUNT_UI, second);
		
		if (90 <= m_timer && m_timer <= 210)
		{
   			AudioManager::GetInstance()->PlaySE("GAME_COUNT1");
		}
		//残り１秒で弾UIを表示
		const int showBulletUISecond = 1;
		if (second == showBulletUISecond)
		{

			//弾UI表示イベントをトリガー
			eventManager->TriggerEvent<std::pair<int, bool>>(
				EventType::SET_BULLET_UI_ACTIVE,
				{ players[0]->GetTeamID(), true}
			);
			eventManager->TriggerEvent<std::pair<int, bool>>(
				EventType::SET_BULLET_UI_ACTIVE,
				{ players[2]->GetTeamID(), true}
			);
		
			//プレイヤーポインター画像非アクティブ化イベントをトリガー
			eventManager->TriggerEvent(EventType::INACTIVATE_PLAYER_POINTER_IMAGES);
		}
	}

	//カウントダウン終了後、ゲーム状態をプレイに変更
	if (m_timer >= COUNTDOWN_DURATION + COUNTDOWN_START) // 4秒カウントダウン
	{
		m_gameState = GameState::STATE_PLAY; // ゲーム状態をプレイに変更
		EventManager::GetInstance()->TriggerEvent(EventType::SHOW_START_UI);
		AudioManager::GetInstance()->PlaySE("GAME_COUNT2");
		AudioManager::GetInstance()->PlayBGM("GAME_BGM");
		m_pGameEventManager->Start(); // イベントマネージャー開始
	}
}

//プレイ中の更新処理
void GameScene::PlayUpdate()
{
	m_pPlayerManager->Update();	//プレイヤー管理クラス更新
	m_pFieldManager->Update();	//フィールド管理クラス更新
	m_pBulletManager->Update(); //弾管理クラス更新
	m_pItemManager->Update();	//アイテム管理クラス更新
	m_pGameEventManager->Update(); //イベント管理クラス更新

	m_pBBManager->SetPlayerData(m_pPlayerManager->GetPlayers());	//プレイヤー位置の設定
	m_pBBManager->Update();		//BB管理クラス更新

	if (m_pSceneContext->pInputInfo->key.enter.trigger)
	{
		if (m_pCamera->GetCameraInfo()->position.y == 0.0f)
		{
			m_pCamera->SetPosition({ 10.0f, 30.0f, -1.0f });
		}
		else
		{
			m_pCamera->SetPosition({ 0.0f, 35.0f, -15.0f });
		}
	}

	//勝利条件の判定
	if (m_isGameOver)
	{
		m_gameState = GameState::STATE_RESULT;
		m_timer = 0;
		EventManager::GetInstance()->TriggerEvent(EventType::SHOW_FINISH_UI);
	}
}

//ゲームオーバー時の更新処理
void GameScene::ResultUpdate()
{
	const int WAIT_DURATION = 150; // リザルトUI表示までの待機フレーム数（2.5秒間）

	if (m_timer == 0)
	{
		AudioManager::GetInstance()->StopAll();
		AudioManager::GetInstance()->PlaySE("RESULT");
	}

	m_timer++;



	if (m_timer == WAIT_DURATION)
	{
		EventManager::GetInstance()->TriggerEvent<std::tuple<int, int, int>>(EventType::SHOW_RESULT_UI, {m_winner, m_character1ID, m_character2ID});
		EventManager::GetInstance()->TriggerEvent(EventType::HIDE_COUNT_UI);
		AudioManager::GetInstance()->StopAll();
		AudioManager::GetInstance()->PlayBGM("RESULT_BGM");	
	}
	else if (m_timer > WAIT_DURATION)
	{
		auto& controllers = m_pSceneContext->pInputInfo->controller;
		auto& keyInput = m_pSceneContext->pInputInfo->key;

		for (size_t i = 0; i < 4; ++i)
		{
			auto controller = controllers[i];

			if (controller.anyButton.trigger || keyInput.space.trigger)
			{
				m_pSceneContext->pInputInfo->SetAllControllerVibration(1.0f, 1.0f, 30);
				EventManager::GetInstance()->TriggerEvent(EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_TITLE);
			}
		}
	}
}