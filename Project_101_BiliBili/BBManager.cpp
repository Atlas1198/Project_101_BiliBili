#include "BBManager.h"
#include "Player.h"
#include "GameUIManager.h"
#include "EventManager.h"
#include "AudioManager.h"

using namespace DirectX;

BBManager::BBManager()
{
}

BBManager::~BBManager()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		delete m_BB[i];
	}
	for (int i = 0; i < BB_AREA_NUM; i++)
	{
		delete m_BBAreas[i];
	}
}

void BBManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager,
	CollisionManager& collisionManager
)
{
	for (int i = 0; i < BB_NUM; i++)
	{
		m_BB[i] = new BB(m_pUIManager, m_pCollisionManager);
	}

	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->Initialize();
		m_BB[i]->SetTeamId(i);

		m_isBBEnhanced[i] = false;
	}

	for(int i = 0; i < BB_NUM; i++)
	{
		for(int j = 0; j < BB::PLAYER_NUM; j++)
		{
			auto lineBB = m_BB[i]->GetLineBB();
		}

		auto electricityBB = m_BB[i]->GetElectricityBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (electricityBB[j])
			{
				electricityBB[j]->GetColliderSet()->RegisterColliders(collisionManager);
			}
		}

		m_BB[i]->DisableBB();

		EventManager::GetInstance()->Subscribe<int>(
			EventType::ITEM_PICKUP,
			[this](std::shared_ptr<int> teamID)
			{
				for(auto& index : m_activationCalledBBIndex)
				{
					if(index == *teamID)
					{
						return;
					}
				}
				m_activationCalledBBIndex.push_back(*teamID);
			}
		);
	}

	for (int i = 0; i < BB_AREA_NUM; i++)
	{
		m_BBAreas[i] = new BilibiliArea(
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			i < 2 ? 0 : 1,
			BB::DAMAGE / 3.0f
		);
		m_BBAreas[i]->GetColliderSet()->RegisterColliders(collisionManager);
  		m_BBAreas[i]->SetActive(false);
	}

	EventManager::GetInstance()->Subscribe<void>(
		EventType::SHOW_START_UI,
		[this](std::shared_ptr<void> data)
		{
			bbAreaStartEventTimer.Mark();
			timerStarted = true;
		}
	);

	EventManager::GetInstance()->Subscribe<int>(
		EventType::ENHANCE_BB,
		[this](std::shared_ptr<int> teamID)
		{
			if (*teamID < 0 || *teamID >= BB_NUM)
			{
				return;
			}
			m_isBBEnhanced[*teamID] = true;
		}
	);
}

void BBManager::OnItemPickup(int teamID)
{
	if (teamID < 0 || teamID >= BB_NUM)
	{
		return;
	}
	if (!m_BB[teamID]->IsActivated())
	{
		SetBB(teamID, true);


		if (m_isBBEnhanced)
		{
			m_BBAreas[teamID * 2]->SetActive(true);
			m_BBAreas[teamID * 2 + 1]->SetActive(true);
		}

	}
	m_BBTimer[teamID] = 10.0f;
	m_frameTimer[teamID].Mark();
}

//譖ｴ譁ｰ
void BBManager::UpdateOverride()
{
	//BB発動コマンド処理
	for (auto& index : m_activationCalledBBIndex)
	{   
		OnItemPickup(index);

		AudioManager::GetInstance()->PauseBGM("GAME_BGM");
		AudioManager::GetInstance()->PlayBGM("GAME_TF_BGM");
	}
	m_activationCalledBBIndex.clear();

	/*if(timerStarted && !m_isBBEnhanced && bbAreaStartEventTimer.Peek() >= BB_ENHANCE_TIME)
	{
		m_isBBEnhanced = true;
		EventManager::GetInstance()->TriggerEvent<EventType>(SHOW_ANOUNCE_UI, EVENT_BB_ENHANCE);
	}*/

	//BB時間管理
	for(int i = 0; i < BB_NUM; i++)
	{
		if (m_BBTimer[i] > 0.0f)
		{
			m_BBTimer[i] -= m_frameTimer[i].Mark();

			if (m_BBTimer[i] <= 0.0f)
			{
				// BB無効化処理
				SetBB(i, false);
				m_BBTimer[i] = 0.0f;
				m_BBAreas[i * 2]->SetActive(false);
				m_BBAreas[i * 2 + 1]->SetActive(false);

				AudioManager::GetInstance()->StopBGM("GAME_TF_BGM");
				AudioManager::GetInstance()->ResumeBGM("GAME_BGM");
				AudioManager::GetInstance()->StopLoopSE("TF_SHOOT");
			}
			
		}

		m_BB[i]->Update();
		m_BBAreas[i * 2]->Update();
		m_BBAreas[i * 2 + 1]->Update();
	}
}

void BBManager::SubmitDrawsOverride(Renderer& renderer)
{

	for(int i = 0; i < BB_NUM; i++)
	{
		auto lineBB = m_BB[i]->GetLineBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (lineBB[j])
			{
				SubmitRenderInfo(renderer, *lineBB[j], m_LineBBInfo);
			}
		}
		auto electricityBB = m_BB[i]->GetElectricityBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (electricityBB[j])
			{
				SubmitRenderInfo(renderer, *electricityBB[j], 
					i == 0 ? m_ElectricityBBBlueInfo : m_ElectricityBBRedInfo);
			}
		}
	}

	for (int i = 0; i < BB_AREA_NUM; i++)
	{
		SubmitRenderInfo(renderer, *m_BBAreas[i], i < 2 ? m_BBAreaBlueInfo : m_BBAreaRedInfo);
	}
}

void BBManager::ResolveCollisionsOverride()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->ResolveCollisions();
	}
	for (int i = 0; i < BB_AREA_NUM; i++)
	{
		m_BBAreas[i]->ResolveCollisions();
	}
}

void BBManager::FinalizeOverride()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->Finalize();
	}

	for(int i = 0; i < BB_AREA_NUM; i++)
	{
		delete m_BBAreas[i];
	}

	timerStarted = false;
}

void BBManager::SetPlayerData(std::vector<Player*>& players)
{

	std::vector<XMFLOAT3> team1Pos;	//チーム1のプレイヤー位置
	std::vector<XMFLOAT3> team2Pos;	//チーム2のプレイヤー位置

	for(auto& player : players)
	{
		if(player->GetTeamID() == 0)
		{
			team1Pos.push_back(player->GetPosition());
			//team1Transformed |= player->IsTransformed();
		}
		else if(player->GetTeamID() == 1)
		{
			team2Pos.push_back(player->GetPosition());
			//team2Transformed |= player->IsTransformed();
		}
	}

	m_BB[0]->SetPlayerPos(team1Pos.data());
	m_BB[1]->SetPlayerPos(team2Pos.data());

	m_BBAreas[0]->SetPlayerPos(team1Pos[0]);
	m_BBAreas[1]->SetPlayerPos(team1Pos[1]);
	m_BBAreas[2]->SetPlayerPos(team2Pos[0]);
	m_BBAreas[3]->SetPlayerPos(team2Pos[1]);
}

void BBManager::SetBB(int teamID, bool activate)
{
	if(teamID < 0 || teamID >= BB_NUM)
	{
		return;
	}
	if(activate)
	{
		m_BB[teamID]->ActivateBB();
		EventManager::GetInstance()->TriggerEvent<std::pair<int, bool>>(EventType::SET_BB, std::make_pair(teamID, true));
	}
	else
	{
		m_BB[teamID]->DisableBB();
		EventManager::GetInstance()->TriggerEvent<std::pair<int, bool>>(EventType::SET_BB, std::make_pair(teamID, false));
	}
}

void BBManager::SetGameUIManager(GameUIManager* pUIManager)
{
	m_pUIManager = pUIManager;
}

void BBManager::SetCollisionManager(CollisionManager* pCollisionManager)
{
	m_pCollisionManager = pCollisionManager;
}

void BBManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_LineBBInfo,
		m_BB[0]->GetLineBB()[0]->GetMeshType(),
		PSO_KEY_MASKED,
		lineBBTexPath,
		false
	);

	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_ElectricityBBBlueInfo,
		m_BB[0]->GetElectricityBB()[0]->GetMeshType(),
		PSO_KEY_TRANSPARENT,
		electricityBBBlueTexPath,
		false
	);

	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_ElectricityBBRedInfo,
		m_BB[0]->GetElectricityBB()[0]->GetMeshType(),
		PSO_KEY_TRANSPARENT,
		electricityBBRedTexPath,
		false
	);

	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_BBAreaBlueInfo,
		m_BBAreas[0]->GetMeshType(),
		PSO_KEY_TRANSPARENT,
		areaBBBlueTexPath,
		false,
		BILLBOARD_FIX_X
	);

	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_BBAreaRedInfo,
		m_BBAreas[0]->GetMeshType(),
		PSO_KEY_TRANSPARENT,
		areaBBRedTexPath,
		false,
		BILLBOARD_FIX_X
	);
}
