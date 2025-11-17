#include "PlayerManager.h"
#include "AssimpLoader.h"
#include "Engine.h"
#include "TextureManager.h"
#include "Collider.h"

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
	//プレイヤーオブジェクトの生成と初期化
	for (int i = 0; i < PLAYER_NUM; i++)
	{
		//プレイヤーオブジェクトの生成
		m_pPlayer[i] = new Player
		(
			MESH_TYPE::QUAD,			//メッシュタイプ
			XMFLOAT3(i* 5.0f - 5.0f, 0.0f, 0.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(2.0f, 2.0f, 2.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::SPHERE,		//コライダータイプ
			XMFLOAT3(2.0f, 2.0f, 2.0f),	//コライダーのボックスサイズ
			false						//コライダーのトリガーフラグ
		);

		//プレイヤーオブジェクトの初期化
		m_pPlayer[i]->Initialize(pInputManager);	//入力情報構造体の取得

		//コライダー情報をシーンに提出
		SubmitColliders(collisionManager, m_pPlayer[i]->GetCollider());
	}
}

//更新
void PlayerManager::Update()
{
	////プレイヤーオブジェクトの更新
	//for (int i = 0; i < PLAYER_NUM; i++)
	//{
	//	m_pPlayer[i]->Update();
	//}

	m_pPlayer[0]->Update();
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
	CreteRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_playerInfo,					//描画情報構造体配列へのポインタ
		m_pPlayer[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		texPath							//テクスチャのファイル名
	);
}