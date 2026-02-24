#include "ItemManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"
#include <random>
#include "EventManager.h"
#include "AudioManager.h"

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
}

void ItemManager::SpawnItem()
{
	static std::mt19937 rng{ std::random_device{}() };  // 乱数エンジン（1回だけ生成して使い回す）
	std::uniform_int_distribution<int> areaDist(0, 2);  // エリア番号を等確率で抽選（0～2）
	const int area = areaDist(rng);                     // 抽選したエリア番号
	
	float xMin = 0.0f, xMax = 0.0f; // X範囲（後でswitchで決める）
	float zMin = 0.0f, zMax = 0.0f; // Z範囲（後でswitchで決める）

	switch (m_pSceneContext->stageType)
	{
	case STAGE_TYPE::STAGE_GREEN:
		switch (area)
		{
		case 0: xMin = -13.0f; xMax = -3.0f;  zMin = -8.0f;  zMax = 4.0f;  break;
		case 1: xMin = -2.0f;  xMax = 6.0f;   zMin = 5.0f;   zMax = 18.0f; break;
		case 2: xMin = 7.0f;   xMax = 13.0f;  zMin = -8.0f;  zMax = 18.0f; break;
		default:xMin = -13.0f; xMax = 13.0f;  zMin = -8.0f;  zMax = 18.0f; break;
		}
		break;
	case STAGE_TYPE::STAGE_BLUE:
		switch (area)
		{
		case 0: xMin = -13.0f; xMax = -3.0f;  zMin = -8.0f;  zMax = 4.0f;  break;
		case 1: xMin = -2.0f;  xMax = 6.0f;   zMin = 5.0f;   zMax = 18.0f; break;
		case 2: xMin = 7.0f;   xMax = 13.0f;  zMin = -8.0f;  zMax = 18.0f; break;
		default:xMin = -13.0f; xMax = 13.0f;  zMin = -8.0f;  zMax = 18.0f; break;
		}
		break;
	case STAGE_TYPE::STAGE_RED:
		switch (area)
		{
		case 0: xMin = -13.0f; xMax = -3.0f;  zMin = -8.0f;  zMax = 4.0f;  break;
		case 1: xMin = -2.0f;  xMax = 6.0f;   zMin = 5.0f;   zMax = 18.0f; break;
		case 2: xMin = 7.0f;   xMax = 13.0f;  zMin = -8.0f;  zMax = 18.0f; break;
		default:xMin = -13.0f; xMax = 13.0f;  zMin = -8.0f;  zMax = 18.0f; break;
		}
		break;
	}

	


	std::uniform_real_distribution<float> xDist(xMin, xMax);    // 決まったX範囲で乱数
	std::uniform_real_distribution<float> zDist(zMin, zMax);    // 決まったZ範囲で乱数

	const float x = xDist(rng);          // X座標を抽選
	const float z = zDist(rng);          // Z座標を抽選

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
