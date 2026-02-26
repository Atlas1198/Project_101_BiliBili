#include "ItemManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"
#include "EventManager.h"
#include "AudioManager.h"
#include "StageSelector.h"

using namespace DirectX;


//コンストラクタ
ItemManager::ItemManager()
{
	//TODO: 地面と壁オブジェクトの生成
}

//デストラクタ
ItemManager::~ItemManager()
{
	//地面オブジェクトの解放
	for (auto& item : m_pItems)
	{
		delete item;
		item = nullptr;
	}
	m_pItems.clear();
}

//初期化
void ItemManager::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager, CollisionManager& collisionManager)
{
	m_pCollisionManager = &collisionManager;

	EventManager::GetInstance()->Subscribe<void>(
		EventType::SHOW_START_UI,
		[this](std::shared_ptr<void> data)
		{
			StartTimer();
		}
	);

	applyNewSpawnRate = false;
	nextItemIndex = 1;
	m_pSceneContext->stageType = StageSelector::GetInstance().GetStage();
	m_pRandom = new Random(1);
}

void ItemManager::SpawnItem()
{
	int g_area = m_pRandom->GetInt(0, 4);
	int r_area = m_pRandom->GetInt(0, 6);
	int b_area = m_pRandom->GetInt(0, 9);

	float xMin = 0.0f, xMax = 0.0f; // X範囲（後でswitchで決める）
	float zMin = 0.0f, zMax = 0.0f; // Z範囲（後でswitchで決める）

	switch (m_pSceneContext->stageType)
	{
	case STAGE_TYPE::STAGE_GREEN:
		switch (g_area)
		{
		case 0: xMin = -4.5f; xMax = 4.5f;  zMin = 2.0f;  zMax = 10.0f;  break;
		case 1: xMin = -8.0f;  xMax = -6.0f;   zMin = 8.0f;   zMax = 15.0f; break;
		case 2: xMin = -17.0f;   xMax = -15.0f;  zMin = -3.0f;  zMax = 4.0f; break;
		case 3: xMin = 9.5f;   xMax = 14.5f;  zMin = 10.0f;  zMax = 13.0f; break;
		case 4: xMin = 12.0f;   xMax = 19.0f;  zMin = 0.0f;  zMax = 2.0f; break;
		}
		break;
	case STAGE_TYPE::STAGE_RED:
		switch (r_area)
		{
		case 0: xMin = -15.0f; xMax = -14.0f;  zMin = 13.0f;  zMax = 16.0f;  break;
		case 1: xMin = -16.0f; xMax = -15.0f;  zMin = 3.0f;  zMax = 6.0f;  break;
		case 2: xMin = -10.0f; xMax = -9.0f;  zMin = -3.0f;  zMax = 0.0f;  break;
		case 3: xMin = -1.5f; xMax = 1.5f;  zMin = 2.0f;  zMax = 8.0f;  break;
		case 4: xMin = 10.0f; xMax = 11.0f;  zMin = 12.0f;  zMax = 15.0f;  break;
		case 5: xMin = 15.0f; xMax = 16.0f;  zMin = 4.0f;  zMax = 7.0f;  break;
		case 6: xMin = 13.0f; xMax = 15.0f;  zMin = -6.0f;  zMax = -2.0f;  break;
		}
		break;
	case STAGE_TYPE::STAGE_BLUE:
		switch (b_area)
		{
		case 0: xMin = -5.0f; xMax = -3.0f;  zMin = -3.0f;  zMax = 5.0f;  break;
		case 1: xMin = 3.0f; xMax = 5.0f;  zMin = 5.0f;  zMax = 13.0f;  break;
		case 2: xMin = -1.0f; xMax = 1.0f;  zMin = 13.0f;  zMax = 16.0f;  break;
		case 3: xMin = -5.0f; xMax = -3.0f;  zMin = 10.0f;  zMax = 13.0f;  break;
		case 4: xMin = 3.0f; xMax = 5.0f;  zMin = -3.0f;  zMax = 0.0f;  break;
		case 5: xMin = -1.0f; xMax = 1.0f;  zMin = -6.0f;  zMax = -3.0f;  break;
		case 6: xMin = -11.0f; xMax = -9.0f;  zMin = 10.0f;  zMax = 15.0f;  break;
		case 7: xMin = 9.0f; xMax = 11.0f;  zMin = -3.0f;  zMax = 2.0f;  break;
		case 8: xMin = -15.0f; xMax = -9.0f;  zMin = 6.0f;  zMax = 8.0f;  break;
		case 9: xMin = 9.0f; xMax = 15.0f;  zMin = 4.0f;  zMax = 6.0f;  break;
		}
		break;
	
	}

	


	float xDist = m_pRandom->GetFloat(xMin, xMax);    // 決まったX範囲で乱数
	float zDist = m_pRandom->GetFloat(zMin, zMax);    // 決まったZ範囲で乱数

	const float x = xDist;          // X座標を抽選
	const float z = zDist;          // Z座標を抽選

	m_pItems.push_back(                  // アイテムを配列に追加
		new Item(                        // アイテム生成
			MESH_TYPE::QUAD,			 // メッシュ
			XMFLOAT3(x, 4.0f, z),        // 位置（Yは固定）
			XMFLOAT3(0.0f, 0.0f, 0.0f),  // 回転
			XMFLOAT3(3.5f, 3.5f, 3.5f),  // スケール
			XMFLOAT3(0.0f, -1.0f, 0.0f), // 速度
			true                         // active
		)
	);

	m_pItems.back()->SetColor(XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f));				// 半透明
	m_pItems.back()->GetColliderSet()->RegisterColliders(*m_pCollisionManager); // コライダー登録
}


//更新
void ItemManager::UpdateOverride()
{
	if (!showedAnnouncement &&
		m_frameTimer.Peek() >= (ITEM_RESPAWN * (applyNewSpawnRate ? EVENT_SPAWN_RATE : 1.0f)) - 1.0f)
	{
		bool skipped = false;
		for (int i = 0; i < MAX_SKIPS; i++)
		{
			if (nextItemIndex == skips[i])
			{
				skipped = true;
				break;
			}
		}

		if (!skipped)
		{
			showedAnnouncement = true;
			EventManager::GetInstance()->TriggerEvent<EventType>(EventType::SHOW_ANOUNCE_UI, EventType::EVENT_ITEM_SPAWN);
			AudioManager::GetInstance()->PlaySE("ANNOUNCE_ALL");
		}
	}
	if (m_frameTimer.Peek() >= (ITEM_RESPAWN * (applyNewSpawnRate ? EVENT_SPAWN_RATE : 1.0f)))
	{
		bool skipped = false;

		for (int i = 0; i < MAX_SKIPS; i++)
		{
			if (nextItemIndex == skips[i])
			{
				skipped = true;
				break;
			}
		}

		if (!skipped)
		{
			SpawnItem();
			EventManager::GetInstance()->TriggerEvent<EventType>(EventType::SHOW_ANOUNCE_UI, EventType::EVENT_ITEM_SPAWN);
		}

		showedAnnouncement = false;
		nextItemIndex++;
		m_frameTimer.Mark();
	}

	if (!applyNewSpawnRate && m_totalTimer.Peek() >= 180.0f)
	{
		applyNewSpawnRate = true;
	}

	for(auto& i : m_pItems)
	{
		if (i->IsActive())
		{
			i->Update();
		}
	}
}

//描画要求をシーンに提出
void ItemManager::SubmitDrawsOverride(Renderer& renderer)
{
	//地面描画情報をシーンに提出
	for (auto& item : m_pItems)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*item,		//ゲームオブジェクト配列の参照
			m_itemInfo	//地面描画情報
		);
	}
}

//衝突後処理
void ItemManager::ResolveCollisionsOverride()
{
	for (auto& i : m_pItems)
	{
		if (i->IsActive())
		{
			i->ResolveCollisions();
		}
	}
}

//終了
void ItemManager::FinalizeOverride()
{
	for (auto &item : m_pItems)
	{
		delete item;
		item = nullptr;
	}
	m_pItems.clear();

	m_itemInfo.clear();
}

//オブジェクトの描画情報生成
void ItemManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	PSOKey psoKey = PSO_KEY_MASKED;
	psoKey.psEntry = PS_ID::Basic;
	psoKey.blend = BLEND_ALPHA;

	//描画情報生成関数を呼び出し、描画情報を作成
	CreateRenderInfo(
		textureManager,						//テクスチャマネージャへの参照
		meshManager,						//メッシュマネージャへの参照
		&m_itemInfo,						//描画情報構造体配列へのポインタ
		MESH_TYPE::QUAD,					//メッシュタイプ
		PSO_KEY_MASKED,						//ブレンドモード
		itemTexPath,						//テクスチャのファイル名
		false,								//ライト無効
		BILLBOARD_TYPE::BILLBOARD_SPHERICAL	//ビルボードタイプ
	);
}
