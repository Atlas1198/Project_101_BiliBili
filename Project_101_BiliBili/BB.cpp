#include "BB.h"
#include "Renderer.h"
#include "ObjectManagerBase.h"
#include "GameUIManager.h"
#include "CollisionManager.h"

using namespace DirectX;

//コンストラクタ
BB::BB(GameUIManager* pUIManager, CollisionManager* pCollisionManager)
	: m_pUIManager(pUIManager), m_pCollisionManager(pCollisionManager)
{
}

//デストラクタ
BB::~BB()
{
}

//初期化
void BB::Initialize()
{
	m_lineBB = new LineBB(
		MeshData::MESH_TYPE::QUAD,
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//座標
		DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f),	//回転
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),	//スケール
		DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
		true,									//アクティブフラグ
		ColliderType::CAPSULE,					//コライダータイプ	
		DirectX::XMFLOAT3(0.1f, 1.0f, 0.2f),	//コライダーボックスサイズ
		false									//コライダーのトリガーフラグ
	);

	for (int i = 0; i < PLAYER_NUM; i++)
	{
		m_electricityBB[i] = new ElectricityBB(
			MeshData::MESH_TYPE::QUAD,
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//座標
			DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f),	//回転
			DirectX::XMFLOAT3(0.2f, 1.0f, 0.2f),	//スケール
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,									//アクティブフラグ
			ColliderType::CAPSULE,					//コライダータイプ	
			DirectX::XMFLOAT3(0.5f, 1.0f, 0.5f),	//コライダーボックスサイズ
			false									//コライダーのトリガーフラグ
		);

		m_electricityBB[i]->SetColor(XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f));	//色設定(黄色)
	}

	m_activatedBB = true;
}

//更新
void BB::Update()
{
	m_lineBB->SetEdgePos(
		m_playerPos[0],
		m_playerPos[1]
	);
	m_lineBB->Update();

	if (m_activatedBB)
	{
		ControlElectricity();

		for (auto& eb : m_electricityBB)
		{
			if (eb->IsActive()) eb->Update();
		}
	}
}

//衝突解決
void BB::ResolveCollisions()
{
	m_lineBB->ResolveCollisions();

	if (m_activatedBB)
	{
		for (auto& eb : m_electricityBB)
		{
			if (eb->IsActive())
			{
				eb->ResolveCollisions();
			}
		}

		//ダメージ処理
		for (auto& eb : m_electricityBB)
		{
			//アクティブかどうか確認
			if (!eb->IsActive()) continue;

			//プレイヤーに当たったかどうか取得
			if (!eb->HasHitPlayer()) continue;

			//ダメージ処理
			m_pUIManager->TakeDamage(
				1 - m_teamId,
				BB::DAMAGE * eb->GetHitNum()
			);

			//リセット
			eb->SetHasHitPlayer(false);
			eb->SetHitNum(0);
		}

	}
}

//終了
void BB::Finalize()
{
	delete m_lineBB;
	m_lineBB = nullptr;
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		delete m_electricityBB[i];
		m_electricityBB[i] = nullptr;
	}
}

//終了
LineBB* BB::GetLineBB() const
{
	return m_lineBB;
}

//電気BB配列の取得
ElectricityBB** BB::GetElectricityBB()
{
	return m_electricityBB;
}

//発動中かどうか取得
bool BB::IsActivated() const
{
	return m_activatedBB;
}

//プレイヤー位置の設定
void BB::SetPlayerPos(DirectX::XMFLOAT3 position[PLAYER_NUM])
{
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		m_playerPos[i] = position[i];
	}
}

//チームIDの設定
void BB::SetTeamId(int id)
{
	m_teamId = id;
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		m_electricityBB[i]->SetTeamId(m_teamId);

		m_lineBB->SetColor(
			XMFLOAT4(
				0.0f,
				static_cast<float>(m_teamId),
				static_cast<float>((m_teamId + 1) % 2),
				1.0f)
		);
	}
}

//ビリビリの有効活
void BB::ActivateBB()
{
	m_activatedBB = true;
	m_electricityBB[0]->SetActive(true);
}

//ビリビリの無効化
void BB::DisableBB()
{
	m_activatedBB = false;
	for (auto& eb : m_electricityBB)
	{
		eb->SetActive(false);
	}
}

//電流の操作
void BB::ControlElectricity()
{
	//ラインと壁の衝突位置配列を取得
	std::vector<XMFLOAT3> lineWallCollisionPoints = m_lineBB->GetWallCollisionPoints();

	if (lineWallCollisionPoints.empty())
	{//壁との衝突がない場合
		//片方の電流をオフ
		if (m_electricityBB[1]->IsActive())
		{
			m_electricityBB[1]->SetActive(false);
		}

		//もう片方に情報を受け渡し
		m_electricityBB[0]->SetStartPos(m_playerPos[0]);			//開始地点
		m_electricityBB[0]->SetEndPos(m_playerPos[1]);				//終了地点
		m_electricityBB[0]->SetRotation(m_lineBB->GetRotation());	//回転
	}
	else
	{//壁との衝突がある場合
		if (!m_electricityBB[1]->IsActive())
		{
			m_electricityBB[1]->SetActive(true);	//両方の電流をオン
			m_pCollisionManager->RegisterCollider(m_electricityBB[1]->GetCollider());	//衝突マネージャーに登録
		}

		//各プレイヤー座標、最も近い衝突点、回転角を受け渡し
		m_electricityBB[0]->SetStartPos(m_playerPos[0]);												//開始地点
		m_electricityBB[0]->SetEndPos(GetClosestCollisionPos(m_playerPos[0], lineWallCollisionPoints));	//終了地点
		m_electricityBB[0]->SetRotation(m_lineBB->GetRotation());										//回転

		m_electricityBB[1]->SetStartPos(m_playerPos[1]);												//開始地点
		m_electricityBB[1]->SetEndPos(GetClosestCollisionPos(m_playerPos[1], lineWallCollisionPoints));	//終了地点
		m_electricityBB[1]->SetRotation(m_lineBB->GetRotation());										//回転
	}
}

//プレイヤー座標から最も近い座標を取得
XMFLOAT3 BB::GetClosestCollisionPos(
	DirectX::XMFLOAT3 position,
	std::vector<DirectX::XMFLOAT3> collisionPointList
)
{
	float closestLen = 0.0f;
	XMFLOAT3 closestPos{};

	for (auto& cp : collisionPointList)
	{
		float len = LengthBetween(position, cp);
		if (len < closestLen || closestLen == 0.0f)
		{
			closestLen = len;
			closestPos = cp;
		}
	}

	return closestPos;
}
