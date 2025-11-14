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
	for (Player* player : m_pPlayer)
	{
		delete player;
	}
}

//初期化
void PlayerManager::Initialize(
	InputManager *pInputManager,		//入力マネージャーのポインタ
	TextureManager& textureManager,		//テクスチャ管理クラスの参照
	MeshManager& meshManager,			//メッシュ管理クラスの参照
	CollisionManager &collisionManager	//衝突管理クラスの参照
)
{
	//プレイヤー描画情報生成
	PrepareRenderInfo(textureManager, meshManager);
}

void PlayerManager::AddPlayer(
	uint32_t id,						//ID
	InputManager *pInputManager,		//入力マネージャーのポインタ
	CollisionManager &collisionManager	//衝突管理クラスの参照
)
{
	Vec3 spawnPos = App::GetInstance()->spawnPos;

	//プレイヤーオブジェクトの生成
	m_pPlayer.push_back(
		new Player
		(
			XMFLOAT3(spawnPos.x, spawnPos.y, spawnPos.z),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(2.0f, 2.0f, 2.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			id,						//自分ですか
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(2.0f, 2.0f, 2.0f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);

	uint32_t selfID = App::GetInstance()->descPlayer.uniqueID;

	//プレイヤーオブジェクトの初期化
	if (id == selfID)
	{
		m_pPlayer.back()->Initialize(pInputManager); //入力情報構造体の取得
	}

	//コライダー情報をシーンに提出
	SubmitColliders(collisionManager, m_pPlayer.back()->GetCollider());
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
void PlayerManager::Update()
{
	for (auto player : m_pPlayer)
	{
		player->Update();
	}
}

//衝突後処理
void PlayerManager::ResolveCollisions()
{
	for(auto& player : m_pPlayer)
	{
		player->ResolveCollisions();
	}
}

//プレイヤーオブジェクトを取得
Player* PlayerManager::GetPlayer() const
{
	return m_pPlayer[0];
}

//描画要求をシーンに提出
void PlayerManager::SubmitDraws(Renderer& renderer)
{
	for (auto& player : m_pPlayer)
	{
		//描画要求をシーンに提出
		GameObjectManager::SubmitDraws(
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
	//デフォルトメッシュから描画情報を作成
	CreateRenderInfoFromDefaultMesh(
		textureManager,				//テクスチャ管理クラスの参照
		meshManager,				//メッシュ管理クラスの参照
		&m_playerInfo,				//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::CUBE,	//メッシュタイプ
		texPath						//テクスチャのファイル名
	);
}
