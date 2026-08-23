#include "BB.h"
#include "Renderer.h"
#include "ObjectManagerBase.h"
#include "GameUIManager.h"
#include "CollisionManager.h"
#include "EventManager.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
BB::BB(GameUIManager* pUIManager, CollisionManager* pCollisionManager)
	: m_pUIManager(pUIManager), m_pCollisionManager(pCollisionManager)
{
}

//デストラクタ
BB::~BB()
{
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		delete m_lineBB[i];
		m_lineBB[i] = nullptr;
		delete m_electricityBB[i];
		m_electricityBB[i] = nullptr;
	}
}

//初期化
void BB::Initialize()
{
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		m_lineBB[i] = new LineBB(
			MESH_TYPE::QUAD,
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//座標
			DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f),	//回転
			DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),	//スケール
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,									//アクティブフラグ
			ColliderType::CAPSULE					//コライダータイプ	
		);

		m_lineBB[i]->SetCollisionManager(m_pCollisionManager);	//衝突マネージャー設定
		m_lineBB[i]->GetColliderSet()->SetActive(false);		//最初はコライダーを無効化

		m_electricityBB[i] = new ElectricityBB(
			MESH_TYPE::QUAD,
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//座標
			DirectX::XMFLOAT3(90.0f, 0.0f, 0.0f),	//回転
			DirectX::XMFLOAT3(6.0f, 1.0f, 1.0f),	//スケール
			DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,									//アクティブフラグ
			ColliderType::CAPSULE					//コライダータイプ	
		);

		m_electricityBB[i]->GetColliderSet()->AddCollider(
			ColliderType::CAPSULE,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.5f, 1.0f, 3.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_electricityBB[i]->GetColliderSet()->RegisterColliders(*m_pCollisionManager);
		m_electricityBB[i]->GetColliderSet()->SetActive(false);

		m_electricityBB[i]->SetTexSplitInfo({
				0,
				8,
				4,
				30,
				0,
				3
			});
	}

	m_activatedBB = true;
}

//更新
void BB::Update()
{
	//ラインBBの更新
	for(int i = 0; i < PLAYER_NUM; i++)
	{
		m_lineBB[i]->SetEdgePos(
			m_playerPos[i],		//開始地点(自分のプレイヤー)
			m_playerPos[1 - i]	//終了地点(反対のプレイヤー)
		);
		m_lineBB[i]->Update();
	}

	m_rotation = m_lineBB[0]->GetRotation();

	//電気BBの更新
	if (m_activatedBB)
	{
		ControlElectricity();

		for (auto& eb : m_electricityBB)
		{
			if(eb->IsActive()) eb->Update();
		}
	}

	ElectricityTexSplitUpdate();
}

//衝突解決
void BB::ResolveCollisions()
{
	for(auto& lb : m_lineBB)
	{
		lb->ResolveCollisions();
	}

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
			EventManager::GetInstance()->TriggerEvent(EventType::TAKE_DAMAGE, std::pair<int, float>(
				1 - m_teamId,
				BB::DAMAGE * eb->GetHitNum()
			));

			//リセット
			eb->SetHasHitPlayer(false);
			eb->SetHitNum(0);
		}
	}
}

//終了
void BB::Finalize()
{
}

//終了
LineBB** BB::GetLineBB()
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

		DirectX::XMFLOAT4 color = XMFLOAT4( 1.0f, 1.0f, 1.0f, 1.0f );

		if(m_teamId == 0)
		{
			color = XMFLOAT4(0.188f, 0.78f, 1.0f, 1.0f); //青
		}
		else if(m_teamId == 1)
		{
			color = XMFLOAT4(1.0f, 0.188f, 0.188f, 1.0f); //赤
		}

		m_lineBB[i]->SetColor(color);
	}
}

//ビリビリの有効化
void BB::ActivateBB()
{
	m_activatedBB = true;									//発動中フラグを立てる
	m_electricityBB[0]->SetActive(true);					//片方の電流をオン
	m_electricityBB[0]->GetColliderSet()->SetActive(true);	//コライダーもオン
	for(auto& line : m_lineBB)
	{
		line->SetDrawn(false);		//ラインの描画をオフ
	}
	//シーンエフェクトのフラグをオン
	EventManager::GetInstance()->TriggerEvent<bool>(EventType::SET_BB_SCENE_EFFECT, true);
	//画面振動
	EventManager::GetInstance()->TriggerEvent<std::pair<int, float>>(
		EventType::CALL_CAMERA_SHAKE, std::make_pair(30, 10.0f)
	);
}

//ビリビリの無効化
void BB::DisableBB()
{
	m_activatedBB = false;
	for (auto& eb : m_electricityBB)
	{
		eb->SetActive(false);
	}
	for (auto& line : m_lineBB)
	{
		line->SetDrawn(true);		//ラインの描画をオン
	}

}

//電流の操作
void BB::ControlElectricity()
{
	//ラインと壁の衝突位置配列を取得(距離の昇順でソート済み)
	auto lineCollisionInfos0 = m_lineBB[0]->GetRaycastSegment().hitInfos;
	bool wallCollision = false;
	for(auto& info : lineCollisionInfos0)
	{
		if (info.opponent->GetOwnerTag() == OBJECT_TAG::WALL || info.opponent->GetOwnerTag() == OBJECT_TAG::WALLCURVE)
		{
			wallCollision = true;
			break;
		}
	}

	if(!wallCollision)
	{//衝突情報がない場合
		//片方の電流をオフ
		if (m_electricityBB[1]->IsActive())
		{
			m_electricityBB[1]->SetActive(false);
			m_electricityBB[1]->GetColliderSet()->SetActive(false);
		}
		//もう片方に情報を受け渡し
		m_electricityBB[0]->SetStartPos(m_playerPos[0]);				//開始地点
		m_electricityBB[0]->SetEndPos(m_playerPos[1]);					//終了地点
		m_electricityBB[0]->SetRotation(m_lineBB[0]->GetRotation());	//回転
		return;
	}
	else
	{
		if (!m_electricityBB[1]->IsActive())
		{
			m_electricityBB[1]->SetActive(true);					//両方の電流をオン
			m_electricityBB[1]->GetColliderSet()->SetActive(true);	//コライダーもオン
		}

		//各プレイヤー座標、最も近い衝突点、回転角を受け渡し
		m_electricityBB[0]->SetStartPos(m_playerPos[0]);						//開始地点
		m_electricityBB[0]->SetEndPos(m_lineBB[0]->GetWallCollisionPoint());	//終了地点
		m_electricityBB[0]->SetRotation(m_rotation);							//回転

		m_electricityBB[1]->SetStartPos(m_playerPos[1]);						//開始地点
		m_electricityBB[1]->SetEndPos(m_lineBB[1]->GetWallCollisionPoint());	//終了地点
		m_electricityBB[1]->SetRotation(m_rotation);							//回転
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

//電気テクスチャ分割更新
void BB::ElectricityTexSplitUpdate()
{
	const float ELECTRICITY_TEX_BASE_LENGTH = 40.0f; //電気テクスチャの基準長さ

	for(auto& eb : m_electricityBB)
	{
		m_length = LengthBetween(
			eb->GetStartPos(),
			eb->GetEndPos()
		);

		float lengthRatio = m_length / ELECTRICITY_TEX_BASE_LENGTH;

		lengthRatio = std::clamp(lengthRatio, 0.0f, 1.0f);

		TexSplitInfo info = eb->GetTexSplitInfo();
		info.scaleV = lengthRatio;
		info.offsetV = (1.0f - lengthRatio) * 0.5f;

		eb->SetTexSplitInfo(info);
	}
}
