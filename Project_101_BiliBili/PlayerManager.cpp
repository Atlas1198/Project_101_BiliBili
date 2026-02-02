#include "PlayerManager.h"
#include "AssimpLoader.h"
#include "Engine.h"
#include "TextureManager.h"
#include "Collider.h"
#include "App.h"
#include "EventManager.h"

using namespace DirectX;

PlayerManager::PlayerManager()
{
}

//デストラクタ
PlayerManager::~PlayerManager()
{
	//プレイヤーオブジェクトの解放
	for (auto& player : m_pPlayer)
	{
		delete player;
		player = nullptr;
	}
	m_pPlayer.clear();
}

//初期化
void PlayerManager::InitializeOverride(
	TextureManager& textureManager,		//テクスチャ管理クラスの参照
	MeshManager& meshManager,			//メッシュ管理クラスの参照
	CollisionManager& collisionManager	//衝突管理クラスの参照
)
{
	for (auto it = m_pPlayer.begin(); it != m_pPlayer.end(); it++)
	{
		(*it)->GetColliderSet()->RegisterColliders(collisionManager);
		(*it)->SetSceneContext(m_pSceneContext);
	}

	m_subscribedEvents.push_back(
		EventData{ EventType::TAKE_DAMAGE,
		EventManager::GetInstance()->Subscribe<std::pair<int, float>>(
		EventType::TAKE_DAMAGE,
		[this](std::shared_ptr<std::pair<int, float>> data)
		{
			int teamID = data->first;
			float damage = data->second;
			OnTakeDamage(teamID, damage);
		}
	) });

	m_subscribedEvents.push_back(
		EventData{ EventType::SET_BB,
		EventManager::GetInstance()->Subscribe<std::pair<int, bool>>(
		EventType::SET_BB,
		[this](std::shared_ptr<std::pair<int, bool>> data)
		{
			int teamID = data->first;
			bool isActive = data->second;
			OnSetBB(teamID, isActive);
		}
	) });

	

	//チームの体力を初期化
	for(auto& hp : teamHP)
	{
		hp = 1.0f;
	}

	for (auto& bbActive : teamBBActive)
	{
		bbActive = false;
	}

	//スポーン位置設定
	for (int i = 0; i < 4; i++)
	{
		m_pPlayer[i]->SetPosition(spawnPoses[i]);
	}

	//キャラクターごとの色設定(テスト用)
	for (int i = 0; i < 4; i++)
	{
		XMFLOAT4 color = { 0.0f, 0.0f, 0.0f, 1.0f };

		switch (m_pSceneContext->playersInfo[i].characterID)
		{
		case 0:
			color = XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f); // 赤
			break;
		case 1:
			color = XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f); // 青
			break;
		case 2:
			color = XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f); // 黄
			break;
		case 3:
			color = XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f); // 緑
			break;
		default:
			break;
		}

		m_pPlayer[i]->SetCharacterID(m_pSceneContext->playersInfo[i].characterID);
		m_pPlayer[i]->SetControllerID(m_pSceneContext->playersInfo[i].controllerID);
	}
}

Player* PlayerManager::AddPlayer(
	uint32_t id,						//ID
	InputManager *pInputManager,		//入力マネージャーのポインタ
	CollisionManager &collisionManager,	//衝突管理クラスの参照
	BulletManager *pBulletManager	//弾丸管理クラスの参照
)
{
	XMFLOAT3 spawnPos = spawnPoses[m_pPlayer.size()];

	Player *newPlayer = new Player
	(
		MESH_TYPE::QUAD,
		XMFLOAT3(spawnPos.x, spawnPos.y, spawnPos.z),	//位置
		XMFLOAT3(0.0f, 0.0f, 0.0f),						//回転
		XMFLOAT3(4.5f, 4.5f, 4.5f),						//スケール
		XMFLOAT3(0.0f, 0.0f, 0.0f),						//移動速度
		id,												//ID
		true,											//アクティブフラグ
		ColliderType::BOX,								//コライダータイプ	
		XMFLOAT3(0.5f, 0.5f, 0.5f),						//コライダーセットサイズ
		false											//コライダーのトリガーフラグ
	);

	//プレイヤーオブジェクトの生成
	m_pPlayer.push_back(
		newPlayer
	);


	if (App::GetInstance()->isOnline)
	{
		uint32_t selfID = App::GetInstance()->descPlayer.uniqueID;

		//プレイヤーオブジェクトの初期化
		if (id == selfID)
		{
			m_pPlayer.back()->Initialize(pInputManager->GetInputInfo(), pBulletManager); //入力情報構造体の取得
		}
	}
	else
	{
		//プレイヤーオブジェクトの初期化
		m_pPlayer.back()->Initialize(pInputManager->GetInputInfo(), pBulletManager); //入力情報構造体の取得
	}

	return newPlayer;
}

void PlayerManager::RemovePlayer(uint32_t id)
{
	for (auto it = m_pPlayer.begin(); it != m_pPlayer.end(); it++)
	{
		if ((*it)->id == id)
		{
			delete *it;
			m_pPlayer.erase(it);
			break;
		}
	}
}

void PlayerManager::OnTakeDamage(int teamID, float damage)
{
	teamHP[teamID] -= damage;
	if (teamHP[teamID] < 0.0f)
	{
		teamHP[teamID] = 0.0f;
	}

	EventManager::GetInstance()->TriggerEvent<std::pair<int, float>>(EventType::UPDATE_HP_UI, std::make_pair(teamID, teamHP[teamID]));

	if (teamHP[teamID] <= 0.0f)
	{
		int winningTeamID = (teamID == 0) ? 1 : 0;
		int winnerCharacter1ID = m_pPlayer[winningTeamID * 2]->GetCharacterID();
		int winnerCharacter2ID = m_pPlayer[winningTeamID * 2 + 1]->GetCharacterID();

		EventManager::GetInstance()->TriggerEvent<std::tuple<bool, int, int, int>>(EventType::GAME_OVER, std::make_tuple(true, winningTeamID, winnerCharacter1ID, winnerCharacter2ID));
	}
}

void PlayerManager::OnSetBB(int teamID, bool isActive)
{
	teamBBActive[teamID] = isActive;
	for (auto& player : m_pPlayer)
	{
		if (player->GetTeamID() == teamID)
		{
			player->SetBB(isActive);
		}
	}
	EventManager::GetInstance()->TriggerEvent<std::pair<int, bool>>(
		EventType::SET_BULLET_UI_ACTIVE,
		{ teamID, !isActive }
	);
}

//更新
void PlayerManager::UpdateOverride()
{
	for (auto player : m_pPlayer)
	{
		player->Update();
	}

	EventManager::GetInstance()->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
		m_pPlayer[0]->GetTeamID(), m_pPlayer[0]->GetPosition(), m_pPlayer[1]->GetPosition()
	));

	EventManager::GetInstance()->TriggerEvent<std::tuple<int, XMFLOAT3, XMFLOAT3>>(
		EventType::SET_PLAYER_CHASING_UI_POSITION, std::make_tuple(
		m_pPlayer[2]->GetTeamID(), m_pPlayer[2]->GetPosition(), m_pPlayer[3]->GetPosition()
	));

#ifdef _DEBUG
	{
		auto keyInput = m_pSceneContext->pInputInfo->key;
		if (keyInput.one.trigger)
		{
			OnTakeDamage(0, 100.0f);
		}

		if (keyInput.two.trigger)
		{
			OnTakeDamage(1, 100.0f);
		}
	}
#endif // _DEBUG
}

//衝突後処理
void PlayerManager::ResolveCollisionsOverride()
{
	for(auto& player : m_pPlayer)
	{
		player->ResolveCollisions();
	}
}

//終了
void PlayerManager::FinalizeOverride()
{
}

//プレイヤーオブジェクトを取得
std::vector<Player*>& PlayerManager::GetPlayers()
{
	return m_pPlayer;
}

//描画要求をシーンに提出
void PlayerManager::SubmitDrawsOverride(Renderer& renderer)
{
	for (int i = 0; i < m_pPlayer.size(); i++)
	{
		//描画要求をシーンに提出
		ObjectManagerBase::SubmitRenderInfo(
			renderer,		//シーンの参照
			*m_pPlayer[i],		//ゲームオブジェクト配列の参照
			teamBBActive[m_pPlayer[i]->GetTeamID()] ? m_playerTransformInfo[i] : m_playerInfo[i]	//プレイヤー描画情報
		);
	}
}

//プレイヤー描画情報生成
void PlayerManager::PrepareRenderInfo(
	TextureManager& textureManager,	//テクスチャ管理クラスの参照
	MeshManager& meshManager		//メッシュ管理クラスの参照
	)
{

	wchar_t normalTextures[4][39] = {
		L"asset/texture/player/BLUE_off_CH.png",
		L"asset/texture/player/SKY_off_CH.png",
		L"asset/texture/player/ORANGE_off_CH.png",
		L"asset/texture/player/YELLOW_off_CH.png",
	};

	wchar_t bbTextures[4][38] = {
		L"asset/texture/player/BLUE_on_CH.png",
		L"asset/texture/player/SKY_on_CH.png",
		L"asset/texture/player/ORANGE_on_CH.png",
		L"asset/texture/player/YELLOW_on_CH.png"
	};

	for (int i = 0; i < 4; i++)
	{
		CreateRenderInfo(
			textureManager,					//テクスチャマネージャへの参照
			meshManager,					//メッシュマネージャへの参照
			&m_playerInfo[i],					//描画情報構造体配列へのポインタ
			m_pPlayer[0]->GetMeshType(),	//メッシュタイプ
			BLEND_MODE::BLEND_MASKED,		//ブレンドモード
			normalTextures[i],		//テクスチャのファイル名
			false,							//ライト無効
			BILLBOARD_TYPE::BILLBOARD_FIX_X
		);

		CreateRenderInfo(
			textureManager,					//テクスチャマネージャへの参照
			meshManager,					//メッシュマネージャへの参照
			&m_playerTransformInfo[i],			//描画情報構造体配列へのポインタ
			m_pPlayer[0]->GetMeshType(),	//メッシュタイプ
			BLEND_MODE::BLEND_MASKED,		//ブレンドモード
			bbTextures[i],		//テクスチャのファイル名
			false,							//ライト無効
			BILLBOARD_TYPE::BILLBOARD_FIX_X
		);
	}
}