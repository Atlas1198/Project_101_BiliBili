#include "PlayerManager.h"
#include "AssimpLoader.h"
#include "Engine.h"
#include "TextureManager.h"
#include "Collider.h"
#include "App.h"

using namespace DirectX;
using namespace RenderData;
using namespace MeshData;

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
	InputManager* pInputManager,		//入力マネージャーのポインタ
	TextureManager& textureManager,		//テクスチャ管理クラスの参照
	MeshManager& meshManager,			//メッシュ管理クラスの参照
	CollisionManager& collisionManager	//衝突管理クラスの参照
)
{
	//プレイヤー描画情報生成
	PrepareRenderInfo(textureManager, meshManager);

	for (auto it = m_pPlayer.begin(); it != m_pPlayer.end(); it++)
	{
		//コライダー情報をシーンに提出
		SubmitColliders(collisionManager, (*it)->GetCollider());
	}
}

Player* PlayerManager::AddPlayer(
	uint32_t id,						//ID
	InputManager *pInputManager,		//入力マネージャーのポインタ
	CollisionManager &collisionManager,	//衝突管理クラスの参照
	BulletManager *pBulletManager	//弾丸管理クラスの参照
)
{
	//Vec3 spawnPos = App::GetInstance()->spawnPos[m_pPlayer.size()];

	Vec3 spawnPoses[4] = {
		{-13.0f, 0.0f, 18.0f},
		{13.0f, 0.0f, 18.0f},
		{-13.0f, 0.0f, -8.0f},
		{13.0f, 0.0f, -8.0f}
	};

	Vec3 spawnPos = spawnPoses[m_pPlayer.size()];

	Player *newPlayer = new Player
	(
		MeshData::MESH_TYPE::QUAD,
		XMFLOAT3(spawnPos.x, spawnPos.y, spawnPos.z),	//位置
		XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
		XMFLOAT3(2.0f, 2.0f, 2.0f),	//スケール
		XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
		id,							//ID
		true,						//アクティブフラグ
		ColliderType::BOX,			//コライダータイプ	
		XMFLOAT3(1.0f, 2.0f, 1.0f),	//コライダーボックスサイズ
		false						//コライダーのトリガーフラグ
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
			m_pPlayer.back()->Initialize(pInputManager, pBulletManager); //入力情報構造体の取得
		}
	}
	else
	{
		//プレイヤーオブジェクトの初期化
		m_pPlayer.back()->Initialize(pInputManager, pBulletManager); //入力情報構造体の取得
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

//更新
void PlayerManager::UpdateOverride()
{
	for (auto player : m_pPlayer)
	{
		player->Update();
	}
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
	for (auto& player : m_pPlayer)
	{
		//描画要求をシーンに提出
		ObjectManagerBase::SubmitRenderInfo(
			renderer,		//シーンの参照
			*player,		//ゲームオブジェクト配列の参照
			m_playerInfo	//プレイヤー描画情報
		);
	}
}

//プレイヤー描画情報生成
void PlayerManager::PrepareRenderInfo(
	TextureManager& textureManager,	//テクスチャ管理クラスの参照
	MeshManager& meshManager		//メッシュ管理クラスの参照
	)
{
	//描画情報生成関数を呼び出し、描画情報を作成
	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_playerInfo,					//描画情報構造体配列へのポインタ
		m_pPlayer[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		texPath							//テクスチャのファイル名
	);
}