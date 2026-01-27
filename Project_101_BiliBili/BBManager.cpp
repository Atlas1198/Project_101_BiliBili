#include "BBManager.h"
#include "Player.h"
#include "GameUIManager.h"
#include "EventManager.h"

using namespace DirectX;

//繧ｳ繝ｳ繧ｹ繝医Λ繧ｯ繧ｿ
BBManager::BBManager()
{
}

//繝�せ繝医Λ繧ｯ繧ｿ
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

//蛻晄悄蛹
void BBManager::InitializeOverride(
	InputManager* pInputManager,
	TextureManager& textureManager,
	MeshManager& meshManager,
	CollisionManager& collisionManager
)
{
	//BB逕滓�
	for (int i = 0; i < BB_NUM; i++)
	{
		m_BB[i] = new BB(m_pUIManager, m_pCollisionManager);
	}

	//BB蛻晄悄蛹
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->Initialize();	//蛻晄悄蛹
		m_BB[i]->SetTeamId(i);	//繝√�繝ID險ｭ螳
	}

	//繧ｳ繝ｩ繧､繝繝ｼ縺ｮ謠仙�
	for(int i = 0; i < BB_NUM; i++)
	{
		//繝ｩ繧､繝ｳBB繧ｳ繝ｩ繧､繝繝ｼ謠仙�
		for(int j = 0; j < BB::PLAYER_NUM; j++)
		{
			auto lineBB = m_BB[i]->GetLineBB();
		}

		//髮ｻ豌唯B繧ｳ繝ｩ繧､繝繝ｼ謠仙�
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

		if (bbAreaStartEventTimer.Peek() >= 120.0f)
		{
			m_BBAreas[teamID * 2]->SetActive(true);
			m_BBAreas[teamID * 2 + 1]->SetActive(true);
		}
	}
	m_BBTimer[teamID] = BB_DURATION;
	m_frameTimer[teamID].Mark();
}

//譖ｴ譁ｰ
void BBManager::UpdateOverride()
{
	//BB発動コマンド処理
	for (auto& index : m_activationCalledBBIndex)
	{
		OnItemPickup(index);
	}
	m_activationCalledBBIndex.clear();

	for(int i = 0; i < BB_NUM; i++)
	{
		if (m_BBTimer[i] > 0.0f)
		{
			m_BBTimer[i] -= m_frameTimer[i].Mark();
			if (m_BBTimer[i] <= 0.0f)
			{
				SetBB(i, false);
				m_BBTimer[i] = 0.0f;
				m_BBAreas[i * 2]->SetActive(false);
				m_BBAreas[i * 2 + 1]->SetActive(false);
			}
		}

		m_BB[i]->Update();
		m_BBAreas[i * 2]->Update();
		m_BBAreas[i * 2 + 1]->Update();
	}
}

//謠冗判隕∵ｱよ署蜃ｺ
void BBManager::SubmitDrawsOverride(Renderer& renderer)
{

	for(int i = 0; i < BB_NUM; i++)
	{
		//繝ｩ繧､繝ｳBB謠冗判諠�ｱ謠仙�
		auto lineBB = m_BB[i]->GetLineBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (lineBB[j])
			{
				SubmitRenderInfo(renderer, *lineBB[j], m_LineBBInfo);
			}
		}

		//髮ｻ豌唯B謠冗判諠�ｱ謠仙�
		auto electricityBB = m_BB[i]->GetElectricityBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (electricityBB[j])
			{
				SubmitRenderInfo(renderer, *electricityBB[j], m_ElectricityBBInfo);
			}
		}
	}

	for (int i = 0; i < BB_AREA_NUM; i++)
	{
		SubmitRenderInfo(renderer, *m_BBAreas[i], m_BBAreaInfo);
	}
}

//陦晉ｪ∬ｧ｣豎ｺ
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

//邨ゆｺ
void BBManager::FinalizeOverride()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->Finalize();
	}
}

//繝励Ξ繧､繝､繝ｼ諠�ｱ縺ｮ險ｭ螳
void BBManager::SetPlayerData(std::vector<Player*>& players)
{

	std::vector<XMFLOAT3> team1Pos;	//チーム1のプレイヤー位置
	std::vector<XMFLOAT3> team2Pos;	//チーム2のプレイヤー位置

	//繝励Ξ繧､繝､繝ｼ縺ｮ菴咲ｽｮ繝ｻ螟芽ｺｫ繝輔Λ繧ｰ繧偵メ繝ｼ繝縺斐→縺ｫ蛻�￠繧
	for(auto& player : players)
	{
		if(player->GetTeamID() == 0)
		{//繝√�繝1
			team1Pos.push_back(player->GetPosition());
			//team1Transformed |= player->IsTransformed();
		}
		else if(player->GetTeamID() == 1)
		{//繝√�繝2
			team2Pos.push_back(player->GetPosition());
			//team2Transformed |= player->IsTransformed();
		}
	}

	//BB縺ｫ繝励Ξ繧､繝､繝ｼ菴咲ｽｮ繧定ｨｭ螳
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

//UI繝槭ロ繝ｼ繧ｸ繝｣繝ｼ縺ｮ險ｭ螳
void BBManager::SetGameUIManager(GameUIManager* pUIManager)
{
	m_pUIManager = pUIManager;
}

//陦晉ｪ√�繝阪�繧ｸ繝｣繝ｼ縺ｮ險ｭ螳
void BBManager::SetCollisionManager(CollisionManager* pCollisionManager)
{
	m_pCollisionManager = pCollisionManager;
}

//BB謠冗判諠�ｱ逕滓�
void BBManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//繝ｩ繧､繝ｳBB謠冗判諠�ｱ逕滓�
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_LineBBInfo,
		m_BB[0]->GetLineBB()[0]->GetMeshType(),
		BLEND_MODE::BLEND_MASKED,
		lineBBTexPath,
		false
	);

	//髮ｻ豌唯B謠冗判諠�ｱ逕滓�
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_ElectricityBBInfo,
		m_BB[0]->GetElectricityBB()[0]->GetMeshType(),
		BLEND_MODE::BLEND_MASKED,
		electricityBBTexPath,
		false
	);

	//BB繧ｨ繝ｪ繧｢謠冗判諠�ｱ逕滓�
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_BBAreaInfo,
		m_BBAreas[0]->GetMeshType(),
		BLEND_MODE::BLEND_MASKED,
		electricityBBTexPath
	);
}
