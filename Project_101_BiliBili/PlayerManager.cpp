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

	for (auto &shadow : m_pPlayerShadow)
	{
		delete shadow;
		shadow = nullptr;
	}
	m_pPlayerShadow.clear();

	for (auto &outline : m_pPlayerOutline)
	{
		if (outline)
		{
			delete outline;
			outline = nullptr;
		}
	}
	m_pPlayerOutline.clear();
}

//初期化
void PlayerManager::InitializeOverride(
	TextureManager& textureManager,		//テクスチャ管理クラスの参照
	MeshManager& meshManager,			//メッシュ管理クラスの参照
	CollisionManager& collisionManager	//衝突管理クラスの参照
)
{
	for (int i = 0; i < m_pPlayer.size(); i++)
	{
		m_pPlayer[i]->Reset();
		m_pPlayer[i]->Update();
		m_pPlayer[i]->GetColliderSet()->RegisterColliders(collisionManager);
		m_pPlayer[i]->SetSceneContext(m_pSceneContext);
		m_pPlayer[i]->SetCharacterID(m_pSceneContext->playersInfo[i].characterID);
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
		m_pPlayerOutline[i]->SetPosition(spawnPoses[i]);
		m_pPlayer[i]->BindOutline(m_pPlayerOutline[i]);
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

	PlayerShadow* newShadow = new PlayerShadow(
		MESH_TYPE::QUAD,
		XMFLOAT3(spawnPos.x, spawnPos.y - 0.1f, spawnPos.z),	//位置
		XMFLOAT3(0.0f, 0.0f, 0.0f),						//回転
		XMFLOAT3(4.5f, 4.5f, 4.5f),						//スケール
		XMFLOAT3(0.0f, 0.0f, 0.0f),						//移動速度
		ColliderType::BOX,								//コライダータイプ	
		XMFLOAT3(0.5f, 0.5f, 0.5f),						//コライダーセットサイズ
		newPlayer,										//影の対象プレイヤー
		false											//コライダーのトリガーフラグ
	);

	m_pPlayerShadow.push_back(newShadow);

	PlayerOutline *newOutline = new PlayerOutline(
		MESH_TYPE::QUAD,
		XMFLOAT3(spawnPos.x, spawnPos.y, spawnPos.z),	//位置
		XMFLOAT3(0.0f, 0.0f, 0.0f),						//回転
		XMFLOAT3(5.0f, 4.5f, 5.0f),						//スケール
		XMFLOAT3(0.0f, 0.0f, 0.0f),						//移動速度
		ColliderType::BOX,								//コライダータイプ	
		XMFLOAT3(0.5f, 0.5f, 0.5f),						//コライダーセットサイズ
		false											//コライダーのトリガーフラグ
	);

	m_pPlayerOutline.push_back(newOutline);

	newPlayer->BindOutline(newOutline);

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

	if (teamHP[teamID] <= 0.5f && !sentBBEnhanceEvent[teamID])
	{
		sentBBEnhanceEvent[teamID] = true;
		EventManager::GetInstance()->TriggerEvent<int>(EventType::ENHANCE_BB, teamID);
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

	for (auto shadow : m_pPlayerShadow)
	{
		shadow->Update();
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
		int characterID = m_pPlayer[i]->GetCharacterID();

		ObjectManagerBase::SubmitRenderInfo(
			renderer,		//シーンの参照
			*m_pPlayerOutline[i],		//ゲームオブジェクト配列の参照
			m_outlineInfo[m_pPlayer[i]->GetTeamID()][characterID][teamBBActive[m_pPlayer[i]->GetTeamID()] ? 1 : 0]	//プレイヤー描画情報
		);

		ObjectManagerBase::SubmitRenderInfo(
			renderer,		//シーンの参照
			*m_pPlayerShadow[i],		//ゲームオブジェクト配列の参照
			m_shadowInfo
		);

		//描画要求をシーンに提出
		ObjectManagerBase::SubmitRenderInfo(
			renderer,		//シーンの参照
			*m_pPlayer[i],		//ゲームオブジェクト配列の参照
			teamBBActive[m_pPlayer[i]->GetTeamID()] ? m_playerTransformInfo[characterID] : m_playerInfo[characterID]	//プレイヤー描画情報
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
		L"asset/texture/player/LUMINA_off_CH.png",
		L"asset/texture/player/U-Ti_off_CH.png",
		L"asset/texture/player/VOLT_off_CH.png",
		L"asset/texture/player/NEON_off_CH.png",
	};

	wchar_t bbTextures[4][38] = {
		L"asset/texture/player/LUMINA_on_CH.png",
		L"asset/texture/player/U-Ti_on_CH.png",
		L"asset/texture/player/VOLT_on_CH.png",
		L"asset/texture/player/NEON_on_CH.png"
	};

	wchar_t shadowTexture[] = L"asset/texture/player/shadow_CH.png";

	for (int i = 0; i < 4; i++)
	{

			auto key = PSO_KEY_TRANSPARENT.AddDefines({ SHADER_DEFINE::PS_MULTIPLY_ALPHA_CONTROL });
			key.blend = BLEND_PREMULTIPLY;
			CreateRenderInfo(
				textureManager,					//テクスチャマネージャへの参照
				meshManager,					//メッシュマネージャへの参照
				&m_playerInfo[i],					//描画情報構造体配列へのポインタ
				m_pPlayer[0]->GetMeshType(),	//メッシュタイプ
				key,
				normalTextures[i],		//テクスチャのファイル名
				false,							//ライト無効
				BILLBOARD_TYPE::BILLBOARD_FIX_X
			);

		CreateRenderInfo(
			textureManager,					//テクスチャマネージャへの参照
			meshManager,					//メッシュマネージャへの参照
			&m_playerTransformInfo[i],			//描画情報構造体配列へのポインタ
			m_pPlayer[0]->GetMeshType(),	//メッシュタイプ
			PSO_KEY_MASKED,		//ブレンドモード
			bbTextures[i],		//テクスチャのファイル名
			false,							//ライト無効
			BILLBOARD_TYPE::BILLBOARD_FIX_X
		);

		CreateRenderInfo(
			textureManager,					//テクスチャマネージャへの参照
			meshManager,					//メッシュマネージャへの参照
			&m_shadowInfo,			//描画情報構造体配列へのポインタ
			MESH_TYPE::QUAD,	//メッシュタイプ
			PSO_KEY_TRANSPARENT,		//ブレンドモード
			shadowTexture,		//テクスチャのファイル名
			false,							//ライト無効
			BILLBOARD_TYPE::BILLBOARD_FIX_X
		);

		for (int teamID = 0; teamID < 2; teamID++)
		{
			for (int bbActive = 0; bbActive < 2; bbActive++)
			{
				CreateRenderInfo(
					textureManager,					//テクスチャマネージャへの参照
					meshManager,					//メッシュマネージャへの参照
					&m_outlineInfo[teamID][i][bbActive],			//描画情報構造体配列へのポインタ
					MESH_TYPE::QUAD,	//メッシュタイプ
					teamID == 0 ? PSO_KEY_OUTLINE_BLUE : PSO_KEY_OUTLINE_RED,		//ブレンドモード
					bbActive == 0 ? normalTextures[i] : bbTextures[i],		//テクスチャのファイル名
					false,							//ライト無効
					BILLBOARD_TYPE::BILLBOARD_FIX_X
				);
			}
		}
	}
}