#include "BBManager.h"
#include "Player.h"
#include "GameUIManager.h"
#include "EventManager.h"

using namespace DirectX;

//コンストラクタ
BBManager::BBManager()
{
}

//デストラクタ
BBManager::~BBManager()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		delete m_BB[i];
	}
}

//初期化
void BBManager::InitializeOverride(
	InputManager* pInputManager,
	TextureManager& textureManager,
	MeshManager& meshManager,
	CollisionManager& collisionManager
)
{
	//BB生成
	for (int i = 0; i < BB_NUM; i++)
	{
		m_BB[i] = new BB(m_pUIManager, m_pCollisionManager);
	}

	//BB初期化
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->Initialize();	//初期化
		m_BB[i]->SetTeamId(i);	//チームID設定
	}

	//コライダーの提出
	for(int i = 0; i < BB_NUM; i++)
	{
		//ラインBBコライダー提出
		for(int j = 0; j < BB::PLAYER_NUM; j++)
		{
			auto lineBB = m_BB[i]->GetLineBB();
		}

		//電気BBコライダー提出
		auto electricityBB = m_BB[i]->GetElectricityBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (electricityBB[j])
			{
				SubmitColliders(collisionManager, electricityBB[j]->GetCollider());
			}
		}

		m_BB[i]->DisableBB();

		EventManager::GetInstance()->Subscribe<int>(
			EventType::ITEM_PICKUP,
			[this](std::shared_ptr<int> teamID)
			{
				this->OnItemPickup(*teamID);
			}
		);
	}
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
	}
	m_BBTimer[teamID] = BB_DURATION;
	m_frameTimer[teamID].Mark();
}

//更新
void BBManager::UpdateOverride()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		if (m_BBTimer[i] > 0.0f)
		{
			m_BBTimer[i] -= m_frameTimer[i].Mark();
			if (m_BBTimer[i] <= 0.0f)
			{
				SetBB(i, false);
				m_BBTimer[i] = 0.0f;
			}
		}

		m_BB[i]->Update();
	}
}

//描画要求提出
void BBManager::SubmitDrawsOverride(Renderer& renderer)
{
	for(int i = 0; i < BB_NUM; i++)
	{
		//ラインBB描画情報提出
		auto lineBB = m_BB[i]->GetLineBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (lineBB[j])
			{
				SubmitRenderInfo(renderer, *lineBB[j], m_LineBBInfo);
			}
		}

		//電気BB描画情報提出
		auto electricityBB = m_BB[i]->GetElectricityBB();
		for (int j = 0; j < BB::PLAYER_NUM; j++)
		{
			if (electricityBB[j])
			{
				SubmitRenderInfo(renderer, *electricityBB[j], m_ElectricityBBInfo);
			}
		}
	}
}

//衝突解決
void BBManager::ResolveCollisionsOverride()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->ResolveCollisions();
	}
}

//終了
void BBManager::FinalizeOverride()
{
	for(int i = 0; i < BB_NUM; i++)
	{
		m_BB[i]->Finalize();
	}
}

//プレイヤー情報の設定
void BBManager::SetPlayerData(std::vector<Player*>& players)
{

	std::vector<XMFLOAT3> team1Pos;	//チーム1のプレイヤー位置
	std::vector<XMFLOAT3> team2Pos;	//チーム2のプレイヤー位置
	//bool team1Transformed = false;	//チーム1の変身フラグ
	//bool team2Transformed = false;	//チーム2の変身フラグ

	//プレイヤーの位置・変身フラグをチームごとに分ける
	for(auto& player : players)
	{
		if(player->GetTeamID() == 0)
		{//チーム1
			team1Pos.push_back(player->GetPosition());
			//team1Transformed |= player->IsTransformed();
		}
		else if(player->GetTeamID() == 1)
		{//チーム2
			team2Pos.push_back(player->GetPosition());
			//team2Transformed |= player->IsTransformed();
		}
	}

	//BBにプレイヤー位置を設定
	m_BB[0]->SetPlayerPos(team1Pos.data());
	m_BB[1]->SetPlayerPos(team2Pos.data());

	/*/
	//BBの有効・無効を設定
	if(team1Transformed)
	{
		m_BB[0]->ActivateBB();
	}
	else
	{
		if(m_BB[0]->IsActivated())
		{
			m_BB[0]->DisableBB();
		}
	}
	if(team2Transformed)
	{
		m_BB[1]->ActivateBB();
	}
	else
	{
		if(m_BB[1]->IsActivated())
		{
			m_BB[1]->DisableBB();
		}
	}
	*/
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
	}
	else
	{
		m_BB[teamID]->DisableBB();
	}
}

//UIマネージャーの設定
void BBManager::SetGameUIManager(GameUIManager* pUIManager)
{
	m_pUIManager = pUIManager;
}

//衝突マネージャーの設定
void BBManager::SetCollisionManager(CollisionManager* pCollisionManager)
{
	m_pCollisionManager = pCollisionManager;
}

//BB描画情報生成
void BBManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//ラインBB描画情報生成
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_LineBBInfo,
		m_BB[0]->GetLineBB()[0]->GetMeshType(),
		BLEND_MODE::BLEND_MASKED,
		lineBBTexPath
	);

	//電気BB描画情報生成
	CreateRenderInfo(
		textureManager,
		meshManager,
		&m_ElectricityBBInfo,
		m_BB[0]->GetElectricityBB()[0]->GetMeshType(),
		BLEND_MODE::BLEND_OPAQUE,
		electricityBBTexPath
	);
}
