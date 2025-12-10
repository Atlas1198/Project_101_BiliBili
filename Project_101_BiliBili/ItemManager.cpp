#include "ItemManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"
#include <random>

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
void ItemManager::InitializeOverride(InputManager* pInputManager, TextureManager& textureManager, MeshManager& meshManager, CollisionManager& collisionManager)
{
	m_pCollisionManager = &collisionManager;
}

void ItemManager::SpawnItem()
{

	// X: -15.0f ~ 15.0f, Z: -10.0f ~ 20.0f の範囲でランダムな位置にアイテムを生成

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> xDist(-13.0f, 13.0f);
	std::uniform_real_distribution<> zDist(-8.0f, 18.0f);

	m_pItems.push_back(
		new Item(
			MESH_TYPE::QUAD,
			XMFLOAT3(xDist(gen), -4.0f, zDist(gen)),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(1.0f, 1.0f, 1.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(1.1f, 1.1f, 1.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);

	SubmitColliders(*m_pCollisionManager, m_pItems.back()->GetCollider());
}

//更新
void ItemManager::UpdateOverride()
{
	if (m_frameTimer.Peek() >= ITEM_RESPAWN)
	{
		SpawnItem();
		m_frameTimer.Mark();
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
	m_pItems.clear();

	m_itemInfo.clear();
}

//オブジェクトの描画情報生成
void ItemManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//描画情報生成関数を呼び出し、描画情報を作成
	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_itemInfo,					//描画情報構造体配列へのポインタ
		MESH_TYPE::QUAD,	//メッシュタイプ
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		itemTexPath					//テクスチャのファイル名
	);

}
