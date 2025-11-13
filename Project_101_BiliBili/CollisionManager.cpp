#include "CollisionManager.h"
#include "GameObject.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"

using namespace DirectX;
using namespace RenderData;
using namespace MeshData;

//コンストラクタ
CollisionManager::CollisionManager()
{
}

//デストラクタ
CollisionManager::~CollisionManager()
{
}

//初期化
void CollisionManager::Initialize(
	TextureManager& textureManager,	//テクスチャ管理クラスの参照
	MeshManager& meshManager		//メッシュ管理クラスの参照
)
{
	CreateColliderRenderInfo(	//コライダー描画情報作成
		textureManager,	//テクスチャ管理クラスの参照
		meshManager		//メッシュ管理クラスの参照
	);
}

//描画
void CollisionManager::Draw(Renderer& renderer)
{
	for(auto& collider : m_pCollidersList)
	{
		ColliderType type = collider->GetType();	//コライダータイプ取得

		switch (type)
		{
		case ColliderType::BOX:		//ボックスコライダー
			SubmitDraw(				//描画要求をシーンに提出
				renderer,
				*collider,
				m_colliderRenderInfoBox
			);
			break;
		case ColliderType::SPHERE:	//球コライダー
			SubmitDraw(				//描画要求をシーンに提出
				renderer,
				*collider,
				m_colliderRenderInfoSphere
			);
			break;
		case ColliderType::CAPSULE:	//カプセルコライダー
			SubmitDraw(				//描画要求をシーンに提出
				renderer,
				*collider,
				m_colliderRenderInfoCapsule
			);
			break;
		}
	}
}

//描画要求をシーンに提出
void CollisionManager::SubmitDraw(Renderer& renderer, const Collider& collider, const std::vector<RenderData::RenderInfo>& info)
{
	for (auto& i : info)
	{
		RenderInfo copy = i;	//描画情報構造体のコピー

		copy.world = collider.GetWorldMatrix();				//ワールド行列の取得

		//色設定
		if (collider.isDetected())
		{//衝突時(ownerに衝突情報があるとき)は赤
			copy.color = DRAW_COLOR_DETECTED;
		}
		else
		{//非衝突時は緑
			copy.color = DRAW_COLOR_DEFAULT;
		}

		renderer.Submit(copy);					//描画要求をシーンに提出
	}
}

//衝突判定処理
void CollisionManager::CheckCollisions()
{
	for(auto& collider : m_pCollidersList)
	{
		//各コライダーの衝突情報クリア
		collider->GetOwner()->ClearCollisionInfos();
		//衝突検知フラグOFF
		collider->setDetected(false);
	}

	//ブロードフェーズ
	BroadPhase();
	//ナローフェーズ
	NarrowPhase();

	//ナローフェーズ用配列クリア
	m_pNarrowPhaseColliders.clear();
}

//ブロードフェーズ(衝突可能性のあるコライダーを絞り込む処理)
//AABB同士の簡易当たり判定
//当たっている可能性のあるコライダーをナローフェーズ用配列に追加
void CollisionManager::BroadPhase()
{
	for(int i = 0; i < m_pCollidersList.size(); i++)
	{
		for(int j = i + 1; j < m_pCollidersList.size(); j++)
		{
			//AABB同士の当たり判定
			AABB aabbA = m_pCollidersList[i]->GetAABB();	//コライダーAのAABB取得
			AABB aabbB = m_pCollidersList[j]->GetAABB();	//コライダーBのAABB取得

			//衝突検知
			if (!(aabbA.min.x <= aabbB.max.x && aabbA.max.x >= aabbB.min.x)) continue;	//X軸方向
			if (!(aabbA.min.y <= aabbB.max.y && aabbA.max.y >= aabbB.min.y)) continue;	//Y軸方向
			if (!(aabbA.min.z <= aabbB.max.z && aabbA.max.z >= aabbB.min.z)) continue;	//Z軸方向

			//ナローフェーズ用配列に追加
			CollisionPair pair;							//衝突ペア構造体
			pair.colliderA = m_pCollidersList[i];		//コライダーA
			pair.colliderB = m_pCollidersList[j];		//コライダーB
			m_pNarrowPhaseColliders.push_back(pair);	//ナローフェーズ用配列に追加
		}
	}
}

//ナローフェーズ
void CollisionManager::NarrowPhase()
{
	for(int i = 0; i < m_pNarrowPhaseColliders.size(); i++)
	{
		//コライダーの取得
		Collider* colliderA = m_pNarrowPhaseColliders[i].colliderA;
		Collider* colliderB = m_pNarrowPhaseColliders[i].colliderB;

		//ボックスコライダーの中心点取得
		XMFLOAT3 centerA = colliderA->GetBoxCollider().center;
		XMFLOAT3 centerB = colliderB->GetBoxCollider().center;

		//ボックスコライダーのサイズの取得
		XMFLOAT3 sizeA = colliderA->GetBoxCollider().size;
		XMFLOAT3 sizeB = colliderB->GetBoxCollider().size;

		//ボックスコライダーの最小座標と最大座標の計算
		XMFLOAT3 minA =
		{
			centerA.x - sizeA.x / 2.0f,
			centerA.y - sizeA.y / 2.0f,
			centerA.z - sizeA.z / 2.0f
		};
		XMFLOAT3 maxA =
		{
			centerA.x + sizeA.x / 2.0f,
			centerA.y + sizeA.y / 2.0f,
			centerA.z + sizeA.z / 2.0f
		};
		XMFLOAT3 minB =
		{
			centerB.x - sizeB.x / 2.0f,
			centerB.y - sizeB.y / 2.0f,
			centerB.z - sizeB.z / 2.0f
		};
		XMFLOAT3 maxB =
		{
			centerB.x + sizeB.x / 2.0f,
			centerB.y + sizeB.y / 2.0f,
			centerB.z + sizeB.z / 2.0f
		};

		//衝突検知
		if (!(minA.x <= maxB.x && maxA.x >= minB.x)) continue;	//X軸方向
		if (!(minA.y <= maxB.y && maxA.y >= minB.y)) continue;	//Y軸方向
		if (!(minA.z <= maxB.z && maxA.z >= minB.z)) continue;	//Z軸方向

		//衝突検知フラグON
		colliderA->setDetected(true);
		colliderB->setDetected(true);

		//衝突情報の作成
		//コライダーAとBの両方に衝突情報を追加する
		//コライダーAに渡す衝突情報
		CollisionInfo infoA;							//衝突情報
		infoA.opponent = colliderB;						//衝突相手のコライダー
		infoA.contactPoint =							//衝突点(簡易的に両者の中心点の中間とする)
		{
			(centerA.x + centerB.x) / 2.0f,
			(centerA.y + centerB.y) / 2.0f,
			(centerA.z + centerB.z) / 2.0f
		};
		infoA.contactNormal = { 0.0f, 0.0f, 0.0f };		//法線は省略(必要に応じて計算を追加する)
		infoA.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略(必要に応じて計算を追加する)

		//衝突情報をオーナーオブジェクトに追加
		GameObject* ownerA = colliderA->GetOwner();
		ownerA->AddCollisionInfo(infoA);	


		//コライダーBに渡す衝突情報
		CollisionInfo infoB;							//衝突情報
		infoB.opponent = colliderA;						//衝突相手のコライダー
		infoB.contactPoint =							//衝突点(簡易的に両者の中心点の中間とする)
		{
			(centerA.x + centerB.x) / 2.0f,
			(centerA.y + centerB.y) / 2.0f,
			(centerA.z + centerB.z) / 2.0f
		};
		//法線と貫入深さは省略(必要に応じて計算を追加する)
		infoB.contactNormal = { 0.0f, 0.0f, 0.0f };		//法線は省略
		infoB.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略

		//衝突情報をオーナーオブジェクトに追加
		GameObject* ownerB = colliderB->GetOwner();
		ownerB->AddCollisionInfo(infoB);	


	}
}

//コライダーの登録
void CollisionManager::RegisterCollider(Collider* collider)
{
	m_pCollidersList.push_back(collider);
}

//コライダーの削除
void CollisionManager::RemoveCollider(Collider* collider)
{
}

//コライダーのクリア
void CollisionManager::ClearColliders()
{
	m_pCollidersList.clear();
}

//衝突情報作成
void CollisionManager::CreateCollisionInfo()
{
}

//コライダー描画情報作成
void CollisionManager::CreateColliderRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//ボックスコライダーの描画情報作成
	m_colliderRenderInfoBox.clear();
	CreateRenderInfoFromDefaultMesh(
		textureManager,				//テクスチャ管理クラスの参照
		meshManager,				//メッシュ管理クラスの参照
		&m_colliderRenderInfoBox,	//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::CUBE,	//メッシュタイプ
		texPath						//テクスチャのファイル名
	);

	//球コライダーの描画情報作成
	m_colliderRenderInfoSphere.clear();
	CreateRenderInfoFromDefaultMesh(
		textureManager,					//テクスチャ管理クラスの参照
		meshManager,					//メッシュ管理クラスの参照
		&m_colliderRenderInfoSphere,	//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::SPHERE,	//メッシュタイプ
		texPath							//テクスチャのファイル名
	);

	//カプセルコライダーの描画情報作成
	m_colliderRenderInfoCapsule.clear();
	CreateRenderInfoFromDefaultMesh(
		textureManager,					//テクスチャ管理クラスの参照
		meshManager,					//メッシュ管理クラスの参照
		&m_colliderRenderInfoCapsule,	//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::CAPSULE,	//メッシュタイプ
		texPath							//テクスチャのファイル名
	);
}
