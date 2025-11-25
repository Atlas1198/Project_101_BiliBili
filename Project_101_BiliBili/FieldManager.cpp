#include "FieldManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"

using namespace DirectX;
using namespace RenderData;

//コンストラクタ
FieldManager::FieldManager()
{
	//TODO: 地面と壁オブジェクトの生成
}

//デストラクタ
FieldManager::~FieldManager()
{
	//壁オブジェクトの解放
	for (auto& wall : m_pWalls)
	{
		delete wall;
		wall = nullptr;
	}
	m_pWalls.clear();

	//地面オブジェクトの解放
	for (auto& ground : m_pGrounds)
	{
		delete ground;
		ground = nullptr;
	}
	m_pGrounds.clear();
}

//初期化
void FieldManager::InitializeOverride(InputManager* pInputManager, TextureManager& textureManager, MeshManager& meshManager, CollisionManager& collisionManager)
{
	m_pWalls.push_back(
		new Wall(
			MeshData::MESH_TYPE::CUBE,
			XMFLOAT3(-15.0f, 0.0f, 5.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(2.0f, 10.0f, 30.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(2.1f, 10.1f, 30.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);
	m_pWalls.push_back(
		new Wall(
			MeshData::MESH_TYPE::CUBE,
			XMFLOAT3(15.0f, 0.0f, 5.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(2.0f, 10.0f, 30.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(2.1f, 10.1f, 30.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);
	m_pWalls.push_back(
		new Wall(
			MeshData::MESH_TYPE::CUBE,
			XMFLOAT3(0.0f, 0.0f, 20.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(30.0f, 10.0f, 2.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(30.1f, 10.1f, 2.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);
	m_pWalls.push_back(
		new Wall(
			MeshData::MESH_TYPE::CUBE,
			XMFLOAT3(0.0f, 0.0f, -10.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(30.0f, 10.0f, 2.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(30.1f, 10.1f, 2.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);

	m_pGrounds.push_back(
		new Ground(
			MeshData::MESH_TYPE::QUAD,
			XMFLOAT3(0.0f, -5.0f, 5.0f),	//位置
			XMFLOAT3(90.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(30.0f, 30.0f, 1.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
			true,							//アクティブフラグ
			ColliderType::BOX,				//コライダータイプ
			XMFLOAT3(30.0f, 30.0f, 1.0f),		//コライダーボックスサイズ
			false							//コライダーのトリガーフラグ
		)
	);

	for (auto& wall : m_pWalls)
	{
		SubmitColliders(collisionManager, wall->GetCollider());
	}

	for(auto& ground : m_pGrounds)
	{
		SubmitColliders(collisionManager, ground->GetCollider());
	}
}

//更新
void FieldManager::UpdateOverride()
{
}

//描画要求をシーンに提出
void FieldManager::SubmitDrawsOverride(Renderer& renderer)
{
	//壁描画情報をシーンに提出
	for (auto& wall : m_pWalls)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*wall,			//ゲームオブジェクト配列の参照
			m_wallInfo		//壁描画情報
		);
	}

	//地面描画情報をシーンに提出
	for (auto& ground : m_pGrounds)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*ground,		//ゲームオブジェクト配列の参照
			m_groundInfo	//地面描画情報
		);
	}
}

//衝突後処理
void FieldManager::ResolveCollisionsOverride()
{
}

//終了
void FieldManager::FinalizeOverride()
{
	m_pWalls.clear();
	m_pGrounds.clear();

	m_wallInfo.clear();
	m_groundInfo.clear();
}

//オブジェクトの描画情報生成
void FieldManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//描画情報生成関数を呼び出し、描画情報を作成
	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_wallInfo,					//描画情報構造体配列へのポインタ
		m_pWalls[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_OPAQUE,		//ブレンドモード
		wallTexPath							//テクスチャのファイル名
	);

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_groundInfo,					//描画情報構造体配列へのポインタ
		m_pGrounds[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		groundTexPath					//テクスチャのファイル名
	);

}
