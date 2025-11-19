#include "CollisionManager.h"
#include "ObjectBase.h"
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

//nullptrになっているコライダーをリストから削除
void CollisionManager::CheckColliders()
{
	for (auto it = m_pCollidersList.begin(); it != m_pCollidersList.end();)
	{
		Collider* c = *it;
		if (c->deleteFlag())
		{
			it = m_pCollidersList.erase(it);
			delete c;
		}
		else
		{
			it++;
		}
	}
}

//描画要求をシーンに提出
void CollisionManager::SubmitDraw(
	Renderer& renderer,							//シーンの参照
	Collider& collider,					//コライダー配列
	std::vector<RenderData::RenderInfo>& info	//描画情報構造体
)
{
	std::vector<RenderInfo> submitInfos;		//Rendererへの提出用描画情報構造体配列
	submitInfos.reserve(info.size());			//容量確保
	ObjectBase& object = *collider.GetOwner();	//コライダー所有者オブジェクトの参照取得

	XMFLOAT4 color;	//描画色
	if(collider.isDetected())
	{//衝突時(ownerに衝突情報があるとき)は赤
		color = DRAW_COLOR_DETECTED;
	}
	else
	{//非衝突時は緑
		color = DRAW_COLOR_DEFAULT;
	}

	if (collider.GetType() == ColliderType::CAPSULE)
	{//カプセルメッシュの場合(複数メッシュに分かれているため個別に処理)
		CapsuleVisualDesc desc{};	//カプセルメッシュの記述データ
		float diamiter = (std::max)(collider.GetScale().x, collider.GetScale().z);
		XMFLOAT3 scale =
		{
			diamiter,
			collider.GetScale().y,
			diamiter
		};

		//カプセルメッシュの記述データ設定
		AppendCapsuleRenderInfos(
			desc,					//カプセル描画情報記述子
			collider.GetCenter(),	//位置
			scale,					//スケール
			collider.GetRotation(),	//回転Euler角
			color,					//色
			info,					//入力元描画情報配列
			submitInfos				//出力先描画情報配列
		);
	}
	else
	{//それ以外のメッシュの場合
		//描画情報構造体配列をそのまま提出用配列にコピー
		for (auto& i : info)
		{
			submitInfos.push_back(i);
		}

		//ワールド行列と色を設定
		for (auto& i : submitInfos)
		{
			i.world = collider.GetWorldMatrix();
			i.color = color;
		}
	}

	//位置とブレンドモードを設定
	for (int i = 0; i < submitInfos.size(); i++)
	{
		submitInfos[i].positionW = object.GetPosition();
		submitInfos[i].blendMode = BLEND_TRANSPARENT;
	}

	//描画要求をシーンに提出
	for (auto& i : submitInfos)
	{
		renderer.Submit(i);
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
		collider->SetDetected(false);
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
			bool isCollided = CollisionAABB(	//AABB同士の当たり判定
				m_pCollidersList[i],	//コライダーA
				m_pCollidersList[j]		//コライダーB
			);

			if(isCollided)
			{//衝突の可能性あり
				SendNarrowPhase(	//ナローフェーズ用配列に衝突ペアを追加
					m_pCollidersList[i],	//コライダーA
					m_pCollidersList[j]		//コライダーB
				);
			}
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

		//コライダータイプの取得
		ColliderType typeA = colliderA->GetType();
		ColliderType typeB = colliderB->GetType();

		//コライダータイプに応じた衝突判定関数の呼び出し
		if(typeA == ColliderType::BOX && typeB == ColliderType::BOX)
		{//ボックス対ボックス
			CollisionBoxToBox(colliderA, colliderB);
		}
		else if(typeA == ColliderType::SPHERE && typeB == ColliderType::SPHERE)
		{//球対球
			CollisionSphereToSphere(colliderA, colliderB);
		}
		else if(typeA == ColliderType::CAPSULE && typeB == ColliderType::CAPSULE)
		{//カプセル対カプセル
			CollisionCapsuleToCapsule(colliderA, colliderB);
		}
		else if((typeA == ColliderType::BOX && typeB == ColliderType::SPHERE) ||
				(typeA == ColliderType::SPHERE && typeB == ColliderType::BOX))
		{//ボックス対球
			if(typeA == ColliderType::BOX)
			{
				CollisionBoxToSphere(colliderA, colliderB);
			}
			else
			{
				CollisionBoxToSphere(colliderB, colliderA);
			}
		}
		else if((typeA == ColliderType::BOX && typeB == ColliderType::CAPSULE) ||
				(typeA == ColliderType::CAPSULE && typeB == ColliderType::BOX))
		{//ボックス対カプセル
			if(typeA == ColliderType::BOX)
			{
				CollisionBoxToCapsule(colliderA, colliderB);
			}
			else
			{
				CollisionBoxToCapsule(colliderB, colliderA);
			}
		}
		else if((typeA == ColliderType::SPHERE && typeB == ColliderType::CAPSULE) ||
				(typeA == ColliderType::CAPSULE && typeB == ColliderType::SPHERE))
		{//球対カプセル
			if(typeA == ColliderType::SPHERE)
			{
				CollisionSphereToCapsule(colliderA, colliderB);
			}
			else
			{
				CollisionSphereToCapsule(colliderB, colliderA);
			}
		}
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

//コライダー描画情報作成
void CollisionManager::CreateColliderRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//ボックスコライダーの描画情報作成
	m_colliderRenderInfoBox.clear();
	CreteRenderInfo(
		textureManager,				//テクスチャ管理クラスの参照
		meshManager,				//メッシュ管理クラスの参照
		&m_colliderRenderInfoBox,	//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::CUBE,	//メッシュタイプ
		BLEND_TRANSPARENT,			//ブレンドモード
		texPath						//テクスチャのファイル名
	);

	//球コライダーの描画情報作成
	m_colliderRenderInfoSphere.clear();
	CreteRenderInfo(
		textureManager,					//テクスチャ管理クラスの参照
		meshManager,					//メッシュ管理クラスの参照
		&m_colliderRenderInfoSphere,	//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::SPHERE,	//メッシュタイプ
		BLEND_TRANSPARENT,				//ブレンドモード
		texPath							//テクスチャのファイル名
	);

	//カプセルコライダーの描画情報作成
	m_colliderRenderInfoCapsule.clear();
	CreteRenderInfo(
		textureManager,					//テクスチャ管理クラスの参照
		meshManager,					//メッシュ管理クラスの参照
		&m_colliderRenderInfoCapsule,	//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE::CAPSULE,	//メッシュタイプ
		BLEND_TRANSPARENT,				//ブレンドモード
		texPath							//テクスチャのファイル名
	);
}

//ボックス同士の衝突判定
bool CollisionManager::CollisionAABB(
	Collider* colliderA,	//コライダーA
	Collider* colliderB		//コライダーB
)
{
	//AABB同士の当たり判定
	AABB aabbA = colliderA->GetAABB();	//コライダーAのAABB取得
	AABB aabbB = colliderB->GetAABB();	//コライダーBのAABB取得

	//衝突検知
	if (!(aabbA.min.x <= aabbB.max.x && aabbA.max.x >= aabbB.min.x)) return false;	//X軸方向
	if (!(aabbA.min.y <= aabbB.max.y && aabbA.max.y >= aabbB.min.y)) return false;	//Y軸方向
	if (!(aabbA.min.z <= aabbB.max.z && aabbA.max.z >= aabbB.min.z)) return false;	//Z軸方向

	return true;
}

//ナローフェーズ用配列に衝突ペアを追加
void CollisionManager::SendNarrowPhase(Collider* colliderA, Collider* colliderB)
{
	//ナローフェーズ用配列に追加
	CollisionPair pair;							//衝突ペア構造体
	pair.colliderA = colliderA;					//コライダーA
	pair.colliderB = colliderB;					//コライダーB
	m_pNarrowPhaseColliders.push_back(pair);	//ナローフェーズ用配列に追加
}

//コライダーからOBBを作成
OBB CollisionManager::CreateOBB(Collider* collider)
{
	OBB obb{};	//OBB構造体

	//中心
	auto center = collider->GetCenter();
	obb.center = XMVectorSet(
		center.x,
		center.y,
		center.z,
		0.0f
		);

	//半分のサイズ
	auto scale = collider->GetScale();
	obb.halfSizes = XMFLOAT3(
		scale.x * 0.5f,
		scale.y * 0.5f,
		scale.z * 0.5f
	);

	//各軸の方向ベクトル(ローカル座標系の基底ベクトルを回転させて求める)
	auto rot = collider->GetRotation();
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(rot.x),
		XMConvertToRadians(rot.y),
		XMConvertToRadians(rot.z)
	);

	//ローカル座標系の基底ベクトルを回転させて各軸の方向ベクトルを求める
	obb.axis[0] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f), R));
	obb.axis[1] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), R));
	obb.axis[2] = XMVector3Normalize(XMVector3TransformNormal(XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), R));

	return obb;
}

//コライダーからカプセルセグメントを作成
CapsuleSegment CollisionManager::CreateCapsuleSegment(Collider* collider)
{
	CapsuleSegment seg{};	//カプセルセグメント構造体
	const CapsuleCollider cap = collider->GetCapsuleCollider(); //カプセルコライダー取得
	
	seg.radius = cap.radius;				//半径設定
	seg.pointA = XMLoadFloat3(&cap.pointA);	//端点A設定
	seg.pointB = XMLoadFloat3(&cap.pointB);	//端点B設定

	return seg;
}

//セグメント間の最小距離の二乗を取得
float CollisionManager::GetMinDistanceSquaredSegmentToSegment(
	const DirectX::FXMVECTOR& p0, const DirectX::FXMVECTOR& p1,	//セグメントPの端点
	const DirectX::FXMVECTOR& q0, const DirectX::FXMVECTOR& q1,	//セグメントQの端点
	DirectX::XMVECTOR& outP,									//セグメントP上の最短点
	DirectX::XMVECTOR& outQ										//セグメントQ上の最短点
)
{
	//セグメントPとセグメントQの各種ベクトル計算
	XMVECTOR dP = XMVectorSubtract(p1, p0);	//セグメントPの方向ベクトル
	XMVECTOR dQ = XMVectorSubtract(q1, q0);	//セグメントQの方向ベクトル
	XMVECTOR W = XMVectorSubtract(p0, q0);	//セグメントPの端点p0から見たセグメントQの端点q0へのベクトル

	//各種内積計算
	float a = XMVectorGetX(XMVector3Dot(dP, dP));	//セグメントPの方向ベクトルの長さの二乗
	float b = XMVectorGetX(XMVector3Dot(dP, dQ));	//セグメントPとセグメントQの方向ベクトルの内積
	float c = XMVectorGetX(XMVector3Dot(dQ, dQ));	//セグメントQの方向ベクトルの長さの二乗
	float d = XMVectorGetX(XMVector3Dot(dP, W));	//セグメントPの方向ベクトルとベクトルWの内積
	float e = XMVectorGetX(XMVector3Dot(dQ, W));	//セグメントQの方向ベクトルとベクトルWの内積

	const float EPSILON = 0.0001f;	//ゼロ除算防止用の微小値
	float denom = a * c - b * b;	//分母

	float s, t; //パラメータsとt

	if(denom < EPSILON)
	{//平行な場合
		s = 0.0f;	//セグメントP上の点はp0に固定
		t = e / c;	//セグメントQ上の点を計算
	}
	else
	{//平行でない場合
		s = (b * e - c * d) / denom;	 //セグメントP上の点を計算
		t = (a * e - b * d) / denom;	 //セグメントQ上の点を計算
	}

	//パラメータsとtをセグメントの範囲内にクランプ
	s = (std::max)(0.0f, (std::min)(1.0f, s));
	t = (std::max)(0.0f, (std::min)(1.0f, t));

	//最短点の計算
	outP = XMVectorAdd(p0, XMVectorScale(dP, s)); //セグメントP上の最短点
	outQ = XMVectorAdd(q0, XMVectorScale(dQ, t)); //セグメントQ上の最短点

	//最短距離の二乗の計算
	XMVECTOR diff = XMVectorSubtract(outP, outQ); //最短点同士の差ベクトル
	return XMVectorGetX(XMVector3Dot(diff, diff)); //最短距離の二乗を返す
}

//点とセグメント間の最小距離の二乗を取得
float CollisionManager::GetMinDistanceSquaredPointToSegment(const DirectX::FXMVECTOR& point, const DirectX::FXMVECTOR& segA, const DirectX::FXMVECTOR& segB, DirectX::XMVECTOR& outClosest)
{
	XMVECTOR segDir = XMVectorSubtract(segB, segA);		//セグメントの方向ベクトル
	XMVECTOR toPoint = XMVectorSubtract(point, segA);	//セグメントの端点Aから点へのベクトル

	float segLengthSquared = XMVectorGetX(XMVector3Dot(segDir, segDir)); //セグメントの長さの二乗

	//セグメントの長さが極端に短い場合の処理
	const float epsilon = 0.0001f; //ゼロ除算防止用の微小値
	if (segLengthSquared < epsilon)
	{//セグメントの長さがほぼゼロの場合、端点Aを最短点とする
		outClosest = segA;
		XMVECTOR diff = XMVectorSubtract(point, segA);	//最短点と点の差ベクトル
		return XMVectorGetX(XMVector3Dot(diff, diff));	//最短距離の二乗を返す
	}

	float t = XMVectorGetX(XMVector3Dot(toPoint, segDir)) / segLengthSquared; //パラメータtの計算

	//パラメータtをセグメントの範囲内にクランプ
	t = (std::max)(0.0f, (std::min)(1.0f, t));

	//最短点の計算
	outClosest = XMVectorAdd(segA, XMVectorScale(segDir, t)); //セグメント上の最短点

	//最短距離の二乗の計算
	XMVECTOR diff = XMVectorSubtract(point, outClosest);	//最短点と点の差ベクトル
	return XMVectorGetX(XMVector3Dot(diff, diff));			//最短距離の二乗を返す
}

//点とOBB間の最小距離の二乗を取得
float CollisionManager::GetMinDistanceSquaredPointToOBB(const DirectX::FXMVECTOR& point, const OBB& obb, DirectX::XMVECTOR& outClosest)
{
	XMVECTOR d = XMVectorSubtract(point, obb.center); //点からOBBの中心へのベクトル

	const XMFLOAT3& halfSizes = obb.halfSizes; //OBBの各軸方向の半分のサイズ

	//点のOBBのローカル座標系での位置を計算
	float local[3]; //OBBのローカル座標系での点の位置
	//各軸について内積計算
	for (int i = 0; i < 3; ++i)
	{
		local[i] = XMVectorGetX(XMVector3Dot(d, obb.axis[i]));
	}

	//クランプ処理
	float clamped[3]; //各軸方向にクランプした値
	clamped[0] = (std::max)(-halfSizes.x, (std::min)(halfSizes.x, local[0]));
	clamped[1] = (std::max)(-halfSizes.y, (std::min)(halfSizes.y, local[1]));
	clamped[2] = (std::max)(-halfSizes.z, (std::min)(halfSizes.z, local[2]));

	//最短点の計算
	outClosest = XMVectorAdd(
		obb.center,
		XMVectorAdd(
			XMVectorScale(obb.axis[0], clamped[0]),
			XMVectorAdd(
				XMVectorScale(obb.axis[1], clamped[1]),
				XMVectorScale(obb.axis[2], clamped[2])
			)
		)
	);	

	//最短距離の二乗の計算
	XMVECTOR diff = XMVectorSubtract(point, outClosest);	//最短点と点の差ベクトル
	return XMVectorGetX(XMVector3Dot(diff, diff));			//最短距離の二乗を返す
}

//ボックス対球の衝突判定
void CollisionManager::CollisionBoxToBox(Collider* colliderA, Collider* colliderB)
{
	OBB a = CreateOBB(colliderA);	//コライダーAからOBB作成
	OBB b = CreateOBB(colliderB);	//コライダーBからOBB作成

	XMFLOAT3 ea = a.halfSizes;	//コライダーAの各軸方向の半分のサイズ
	XMFLOAT3 eb = b.halfSizes;	//コライダーBの各軸方向の半分のサイズ

	XMVECTOR tWorld = XMVectorSubtract(b.center, a.center); //コライダーAから見たコライダーBの位置ベクトル

	//コライダーAのローカル座標系で見たコライダーBの位置ベクトルを計算
	float t[3]; //コライダーAのローカル座標系で見たコライダーBの位置ベクトル
	for (int i = 0; i < 3; ++i)
	{//各軸について内積計算
		t[i] = XMVectorGetX(XMVector3Dot(tWorld, a.axis[i]));
	}	

	//回転行列の計算
	float R[3][3];					//各軸の方向ベクトルの内積を格納する配列
	float absR[3][3];				//絶対値を格納する配列
	const float EPSILON = 0.0001f;	//ゼロ除算防止用の微小値
	for(int i = 0; i < 3; i++)
	{
		for(int j = 0; j < 3; j++)
		{
			R[i][j] = XMVectorGetX(XMVector3Dot(a.axis[i], b.axis[j]));	//各軸の方向ベクトルの内積計算
			absR[i][j] = fabsf(R[i][j]) + EPSILON;						//絶対値計算
		}
	}

	//分離軸の判定
	float eaArr[3] = { ea.x, ea.y, ea.z };	//コライダーAの各軸
	float ebArr[3] = { eb.x, eb.y, eb.z };	//コライダーBの各軸

	//コライダーAの各軸
	for (int i = 0; i < 3; ++i)
	{
		float ra = eaArr[i];
		float rb =
			ebArr[0] * absR[i][0] +
			ebArr[1] * absR[i][1] +
			ebArr[2] * absR[i][2];

		if (fabsf(t[i]) > ra + rb) return; // 分離軸あり
	}
	//コライダーBの各軸
	for (int i = 0; i < 3; ++i)
	{
		float ra =
			eaArr[0] * absR[0][i] +
			eaArr[1] * absR[1][i] +
			eaArr[2] * absR[2][i];
		float rb = ebArr[i];

		float tProj = fabsf(
			t[0] * R[0][i] +
			t[1] * R[1][i] +
			t[2] * R[2][i]);

		if (tProj > ra + rb) return; // 分離軸あり
	}

	//交差軸の判定
	//A0 x B0
	{
		float ra = ea.y * absR[2][0] + ea.z * absR[1][0];
		float rb = eb.y * absR[0][2] + eb.z * absR[0][1];
		float tProj = fabs(t[2] * R[1][0] - t[1] * R[2][0]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A0 x B1
	{
		float ra = ea.y * absR[2][1] + ea.z * absR[1][1];
		float rb = eb.x * absR[0][2] + eb.z * absR[0][0];
		float tProj = fabs(t[2] * R[1][1] - t[1] * R[2][1]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A0 x B2
	{
		float ra = ea.y * absR[2][2] + ea.z * absR[1][2];
		float rb = eb.x * absR[0][1] + eb.y * absR[0][0];
		float tProj = fabs(t[2] * R[1][2] - t[1] * R[2][2]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A1 x B0
	{
		float ra = ea.x * absR[2][0] + ea.z * absR[0][0];
		float rb = eb.y * absR[1][2] + eb.z * absR[1][1];
		float tProj = fabs(t[0] * R[2][0] - t[2] * R[0][0]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A1 x B1
	{
		float ra = ea.x * absR[2][1] + ea.z * absR[0][1];
		float rb = eb.x * absR[1][2] + eb.z * absR[1][0];
		float tProj = fabs(t[0] * R[2][1] - t[2] * R[0][1]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A1 x B2
	{
		float ra = ea.x * absR[2][2] + ea.z * absR[0][2];
		float rb = eb.x * absR[1][1] + eb.y * absR[1][0];
		float tProj = fabs(t[0] * R[2][2] - t[2] * R[0][2]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A2 x B0
	{
		float ra = ea.x * absR[1][0] + ea.y * absR[0][0];
		float rb = eb.y * absR[2][2] + eb.z * absR[2][1];
		float tProj = fabs(t[1] * R[0][0] - t[0] * R[1][0]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A2 x B1
	{
		float ra = ea.x * absR[1][1] + ea.y * absR[0][1];
		float rb = eb.x * absR[2][2] + eb.z * absR[2][0];
		float tProj = fabs(t[1] * R[0][1] - t[0] * R[1][1]);
		if (tProj > ra + rb) return; //分離軸あり
	}
	//A2 x B2
	{
		float ra = ea.x * absR[1][2] + ea.y * absR[0][2];
		float rb = eb.x * absR[2][1] + eb.y * absR[2][0];
		float tProj = fabs(t[1] * R[0][2] - t[0] * R[1][2]);
		if (tProj > ra + rb) return; //分離軸あり
	}

	//ここまで来たら衝突検知
	//衝突検知フラグON
	colliderA->SetDetected(true);
	colliderB->SetDetected(true);

	//衝突情報の作成
	CollisionInfo infoA;							//衝突情報
	infoA.opponent = colliderB;						//衝突相手のコライダー
	infoA.contactPoint =							//衝突点(簡易的に両者の中心点の中間とする)
	{
		(XMVectorGetX(a.center) + XMVectorGetX(b.center)) / 2.0f,
		(XMVectorGetY(a.center) + XMVectorGetY(b.center)) / 2.0f,
		(XMVectorGetZ(a.center) + XMVectorGetZ(b.center)) / 2.0f
	};
	infoA.contactNormal = { 0.0f, 0.0f, 0.0f };		//法線は省略(必要に応じて計算を追加する)
	infoA.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略(必要に応じて計算を追加する)
	colliderA->AddCollisionInfo(infoA);	//衝突情報を追加
	CollisionInfo infoB;							//衝突情報
	infoB.opponent = colliderA;						//衝突相手のコライダー
	infoB.contactPoint =							//衝突点(簡易的に両者の中心点の中間とする)
	{
		(XMVectorGetX(a.center) + XMVectorGetX(b.center)) / 2.0f,
		(XMVectorGetY(a.center) + XMVectorGetY(b.center)) / 2.0f,
		(XMVectorGetZ(a.center) + XMVectorGetZ(b.center)) / 2.0f
	};
	infoB.contactNormal = { 0.0f, 0.0f, 0.0f };		//法線は省略
	infoB.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略
	colliderB->AddCollisionInfo(infoB);	//衝突情報を追加
}

//球同士の衝突判定
void CollisionManager::CollisionSphereToSphere(
	Collider* colliderA,	//コライダーA
	Collider* colliderB		//コライダーB
)
{
	//中心点の取得
	XMFLOAT3 centerA = colliderA->GetSphereCollider().center;
	XMFLOAT3 centerB = colliderB->GetSphereCollider().center;

	//半径の取得
	float radiusA = colliderA->GetSphereCollider().radius;
	float radiusB = colliderB->GetSphereCollider().radius;
	float radiusSum = radiusA + radiusB;

	//中心点間の距離の計算
	float dist = sqrtf(
		(centerA.x - centerB.x) * (centerA.x - centerB.x) +
		(centerA.y - centerB.y) * (centerA.y - centerB.y) +
		(centerA.z - centerB.z) * (centerA.z - centerB.z)
	);

	//衝突検知
	if (!(dist <= radiusSum)) return;

	//衝突検知フラグON
	colliderA->SetDetected(true);
	colliderB->SetDetected(true);

	//衝突情報の作成
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
	colliderA->AddCollisionInfo(infoA);	//衝突情報を追加

	CollisionInfo infoB;							//衝突情報
	infoB.opponent = colliderA;						//衝突相手のコライダー
	infoB.contactPoint =							//衝突点(簡易的に両者の中心点の中間とする)
	{
		(centerA.x + centerB.x) / 2.0f,
		(centerA.y + centerB.y) / 2.0f,
		(centerA.z + centerB.z) / 2.0f
	};
	infoB.contactNormal = { 0.0f, 0.0f, 0.0f };		//法線は省略
	infoB.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略
	colliderB->AddCollisionInfo(infoB);	//衝突情報を追加
}

//カプセル同士の衝突判定
void CollisionManager::CollisionCapsuleToCapsule(
	Collider* colliderA,	//コライダーA
	Collider* colliderB		//コライダーB
)
{
	CapsuleSegment segA = CreateCapsuleSegment(colliderA);	//コライダーAからカプセルセグメント作成
	CapsuleSegment segB = CreateCapsuleSegment(colliderB);	//コライダーBからカプセルセグメント作成

	const float epsilon = 0.0001f;	//微小値

	DirectX::XMVECTOR axisA = XMVectorSubtract(segA.pointB, segA.pointA); //コライダーAの軸ベクトル
	DirectX::XMVECTOR axisB = XMVectorSubtract(segB.pointB, segB.pointA); //コライダーBの軸ベクトル

	float lenA = XMVectorGetX(XMVector3Dot(axisA, axisA)); //コライダーAの軸ベクトルの長さ
	float lenB = XMVectorGetX(XMVector3Dot(axisB, axisB)); //コライダーBの軸ベクトルの長さ

	//軸ベクトルの長さが極端に短い場合の処理
	if(lenA < epsilon || lenB < epsilon)
	{//長さが極端に短い場合は球体として扱う
		CollisionSphereToSphere(
			colliderA,
			colliderB
		);
		return;
	}

	//最短距離の二乗を取得
	XMVECTOR closestA, closestB;	//コライダーA・B上の最短点
	float distSq = CollisionManager::GetMinDistanceSquaredSegmentToSegment(
		segA.pointA, segA.pointB,	//セグメントAの端点
		segB.pointA, segB.pointB,	//セグメントBの端点
		closestA,					//セグメントA上の最短点
		closestB					//セグメントB上の最短点
	);

	//衝突検知
	float radiusSum = segA.radius + segB.radius;	//半径の和
	if (!(distSq <= radiusSum * radiusSum)) return;	//衝突なし

	//法線ベクトルの計算
	float dist = sqrtf((std::max)(distSq, epsilon)); //最短距離
	XMVECTOR normal;								//法線ベクトル
	if(dist > epsilon)
	{
		normal = XMVectorScale(
			XMVectorSubtract(closestB, closestA),
			1.0f / dist
		);
	}
	else
	{
		normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //適当な法線ベクトル
	}

	//衝突点の計算
	//コライダーA・B上の最短点の中間に設定
	XMVECTOR contact = XMVectorScale(
		XMVectorAdd(closestA, closestB),
		0.5f
	);

	XMFLOAT3 contactP, normalF;
	XMStoreFloat3(&contactP, contact);	//衝突点
	XMStoreFloat3(&normalF, normal);	//法線ベクトル

	//衝突検知フラグON
	colliderA->SetDetected(true);
	colliderB->SetDetected(true);

	//衝突情報の作成
	CollisionInfo infoA;				//衝突情報
	infoA.opponent = colliderB;			//衝突相手のコライダー
	infoA.contactPoint = contactP;		//衝突点
	infoA.contactNormal = normalF;		//法線
	colliderA->AddCollisionInfo(infoA);	//衝突情報を追加

	CollisionInfo infoB;				//衝突情報
	infoB.opponent = colliderA;			//衝突相手のコライダー
	infoB.contactPoint = contactP;		//衝突点
	infoB.contactNormal = 
	{//反転法線
		-normalF.x,
		-normalF.y,
		-normalF.z
	};									//法線(反転)
	colliderB->AddCollisionInfo(infoB);	//衝突情報を追加
}

//ボックスと球の衝突判定
void CollisionManager::CollisionBoxToSphere(
	Collider* box,		//ボックスコライダー
	Collider* sphere	//スフィアコライダー
)
{
	OBB obb = CreateOBB(box);	//ボックスコライダーからOBB作成

	//OBB情報
	XMFLOAT3 boxCenter;		//OBBの中心点
	XMFLOAT3 boxHalfSizes;	//OBBの各軸方向の半分のサイズ
	XMVECTOR boxAxis[3];	//OBBの各軸の方向ベクトル

	//球情報
	XMFLOAT3 sphereCenter;	//球の中心点
	float sphereRadius;		//球の半径

	//OBB情報の取得
	XMStoreFloat3(&boxCenter, obb.center);	//OBBの中心点取得
	boxHalfSizes = obb.halfSizes;			//OBBの各軸方向の半分のサイズ取得
	for (int i = 0; i < 3; ++i)				//OBBの各軸の方向ベクトル取得
	{
		boxAxis[i] = obb.axis[i];
	}

	//球情報の取得
	sphereCenter = sphere->GetSphereCollider().center;	//球の中心点取得
	sphereRadius = sphere->GetSphereCollider().radius;	//球の半径取得

	//球の中心点をOBBのローカル座標系で表現
	XMVECTOR boxCenterV = XMLoadFloat3(&boxCenter);			//OBBの中心点ベクトル
	XMVECTOR sphereCenterV = XMLoadFloat3(&sphereCenter);	//球の中心点ベクトル

	XMVECTOR d = XMVectorSubtract(sphereCenterV, boxCenterV); //OBBの中心点から球の中心点へのベクトル

	float localX = XMVectorGetX(XMVector3Dot(d, boxAxis[0])); //OBBのローカルX座標
	float localY = XMVectorGetX(XMVector3Dot(d, boxAxis[1])); //OBBのローカルY座標
	float localZ = XMVectorGetX(XMVector3Dot(d, boxAxis[2])); //OBBのローカルZ座標

	//最も近い点をOBBのローカル座標系で計算
	float closestX = (std::max)(-boxHalfSizes.x, (std::min)(boxHalfSizes.x, localX));
	float closestY = (std::max)(-boxHalfSizes.y, (std::min)(boxHalfSizes.y, localY));
	float closestZ = (std::max)(-boxHalfSizes.z, (std::min)(boxHalfSizes.z, localZ));

	//最も近い点と球の中心点の差ベクトルを計算
	float diffX = closestX - localX; //最も近い点と球の中心点のX成分の差
	float diffY = closestY - localY; //最も近い点と球の中心点のY成分の差
	float diffZ = closestZ - localZ; //最も近い点と球の中心点のZ成分の差

	//最短距離の二乗を計算
	float distSq = diffX * diffX + diffY * diffY + diffZ * diffZ;
	float radiusSq = sphereRadius * sphereRadius; //球の半径の二乗

	//衝突検知
	if (distSq > radiusSq) return; //衝突なし

	//最も近い点をワールド座標系で計算
	XMVECTOR closestWorld; //最も近い点のワールド座標系での位置ベクトル
	closestWorld = XMVectorAdd(
		boxCenterV,
		XMVectorAdd(
			XMVectorScale(boxAxis[0], closestX),
			XMVectorAdd(
				XMVectorScale(boxAxis[1], closestY),
				XMVectorScale(boxAxis[2], closestZ)
			)
		)
	);

	//法線ベクトルの計算
	const float epsilon = 0.0001f;						//微小値
	float dist = sqrtf((std::max)(distSq, epsilon));	//最短距離

	XMVECTOR normal; //法線ベクトル

	if (dist > epsilon)
	{//法線ベクトル計算
		normal = XMVectorScale(
			XMVectorSubtract(sphereCenterV, closestWorld),
			1.0f / dist
		);
	}
	else
	{//最短距離が極端に小さい場合
		normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //適当な法線ベクトル
	}

	XMFLOAT3 normalF;						//法線ベクトル格納用
	XMFLOAT3 contactP;						//衝突点格納用
	XMStoreFloat3(&normalF, normal);		//法線ベクトルを格納
	XMStoreFloat3(&contactP, closestWorld); //衝突点を格納

	//衝突検知フラグON
	box->SetDetected(true);
	sphere->SetDetected(true);

	//衝突情報の作成
	CollisionInfo infoBox;								//衝突情報
	infoBox.opponent = sphere;							//衝突相手のコライダー
	infoBox.contactPoint = contactP;					//衝突点(簡易的に両者の中心点の中間とする)
	infoBox.contactNormal = normalF;					//法線は省略(必要に応じて計算を追加する)
	infoBox.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略(必要に応じて計算を追加する)
	box->AddCollisionInfo(infoBox);	//衝突情報を追加

	CollisionInfo infoSphere;						//衝突情報
	infoSphere.opponent = box;						//衝突相手のコライダー
	infoSphere.contactPoint = contactP;				//衝突点(簡易的に両者の中心点の中間とする)
	infoSphere.contactNormal =						//法線反転
	{
		-normalF.x,
		-normalF.y,
		-normalF.z
	};
	infoSphere.penetrationDepth = { 0.0f, 0.0f, 0.0f };	//貫入深さは省略
	sphere->AddCollisionInfo(infoSphere);	//衝突情報を追加
}

//ボックスとカプセルの衝突判定
void CollisionManager::CollisionBoxToCapsule(
	Collider* box,	//コライダーA
	Collider* capsule		//コライダーB
)
{
	OBB obb = CreateOBB(box);									//ボックスコライダーからOBB作成
	CapsuleSegment cupSeg = CreateCapsuleSegment(capsule);		//カプセルコライダーからカプセルセグメント作成

	const float radius = cupSeg.radius; //カプセルの半径

	constexpr int SAMPLE_COUNT = 8; //サンプリング数
	bool isCollided = false;		//衝突検知フラグ

	XMVECTOR collisionPoints = XMVectorZero();	//衝突点の合計ベクトル
	XMVECTOR collisionNormals = XMVectorZero();	//法線ベクトルの合計ベクトル

	XMVECTOR A = cupSeg.pointA;				//カプセルセグメントの端点A
	XMVECTOR B = cupSeg.pointB;				//カプセルセグメントの端点B
	XMVECTOR AB = XMVectorSubtract(B, A);	//カプセルセグメントの方向ベクトル

	for (int i = 0; i < SAMPLE_COUNT; ++i)
	{
		float t = 0.0f;	//パラメータt
		if (SAMPLE_COUNT > 1)
		{//パラメータtを計算
			t = static_cast<float>(i) / static_cast<float>(SAMPLE_COUNT - 1); //パラメータt
		}

		//カプセルセグメント上のサンプリング点を計算
		XMVECTOR sampleCenter = XMVectorAdd(
			A,
			XMVectorScale(AB, t)
		);

		//最短距離の二乗を取得
		XMVECTOR closestPoint; //OBB上の最短点
		float distSq = GetMinDistanceSquaredPointToOBB(
			sampleCenter,	//点(サンプリング点)
			obb,			//OBB
			closestPoint	//OBB上の最短点
		);

		if(distSq <= radius * radius)
		{//衝突検知
			isCollided = true;	//衝突検知フラグON
			collisionPoints = closestPoint;	

			//法線ベクトルの計算
			XMVECTOR diff = XMVectorSubtract(sampleCenter, closestPoint); //最短点とサンプリング点の差ベクトル
			const float epsilon = 0.0001f;						//微小値
			float dist = XMVectorGetX(XMVector3Length(diff));	//最短距離

			if (dist > epsilon)
			{//法線ベクトル計算
				collisionNormals = XMVectorScale(
					diff,
					1.0f / dist
				);
			}
			else
			{
				collisionNormals = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //適当な法線ベクトル
			}

			break;	//最初に衝突した点で処理を抜ける
		}
	}

	if(isCollided)
	{
		XMFLOAT3 contactP, normalF;
		XMStoreFloat3(&contactP, collisionPoints);	//衝突点
		XMStoreFloat3(&normalF, collisionNormals);	//法線ベクトル

		//衝突検知フラグON
		box->SetDetected(true);
		capsule->SetDetected(true);

		//衝突情報の作成
		CollisionInfo infoBox;					//衝突情報
		infoBox.opponent = capsule;				//衝突相手のコライダー
		infoBox.contactPoint = contactP;		//衝突点
		infoBox.contactNormal = normalF;		//法線
		box->AddCollisionInfo(infoBox);			//衝突情報を追加

		CollisionInfo infoCapsule;				//衝突情報
		infoCapsule.opponent = box;				//衝突相手のコライダー
		infoCapsule.contactPoint = contactP;	//衝突点
		infoCapsule.contactNormal =				//反転法線
		{
			-normalF.x,
			-normalF.y,
			-normalF.z
		};
		capsule->AddCollisionInfo(infoCapsule);	//衝突情報を追加
	}
}

//球とカプセルの衝突判定
void CollisionManager::CollisionSphereToCapsule(
	Collider* sphere,	//球コライダー
	Collider* capsule	//カプセルコライダー
)
{
	CapsuleSegment cupSeg = CreateCapsuleSegment(capsule);		//カプセルコライダーからカプセルセグメント作成
	SphereCollider sphereCol = sphere->GetSphereCollider();		//球コライダー情報取得
	XMVECTOR sphereCenter = XMLoadFloat3(&sphereCol.center);	//球の中心点ベクトル
	float radius = sphereCol.radius;							//球の半径

	//最短距離の二乗を取得
	XMVECTOR closestPoint; //カプセルセグメント上の最短点
	float distSq = CollisionManager::GetMinDistanceSquaredPointToSegment(
		sphereCenter,				//点(球の中心点)
		cupSeg.pointA,				//カプセルセグメントの端点A
		cupSeg.pointB,				//カプセルセグメントの端点B
		closestPoint				//カプセルセグメント上の最短点
	);

	float radiusSum = radius + cupSeg.radius;		//半径の和
	if (!(distSq <= radiusSum * radiusSum)) return; //衝突なし

	//衝突検知フラグON
	sphere->SetDetected(true);
	capsule->SetDetected(true);

	//法線ベクトルの計算
	const float epsilon = 0.0001f;						//微小値
	float dist = sqrtf((std::max)(distSq, epsilon));	//最短距離
	XMVECTOR normal;									//法線ベクトル
	if (dist > epsilon)
	{
		normal = XMVectorScale(
			XMVectorSubtract(sphereCenter, closestPoint),
			1.0f / dist
		);
	}
	else
	{
		normal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //適当な法線ベクトル
	}
	//衝突点の計算
	XMVECTOR contact = XMVectorScale(
		XMVectorAdd(sphereCenter, closestPoint),
		0.5f
	);
	XMFLOAT3 contactP, normalF;
	XMStoreFloat3(&contactP, contact);	//衝突点
	XMStoreFloat3(&normalF, normal);	//法線ベクトル

	//衝突情報の作成
	CollisionInfo infoSphere;				//衝突情報
	infoSphere.opponent = capsule;			//衝突相手のコライダー
	infoSphere.contactPoint = contactP;		//衝突点
	infoSphere.contactNormal = normalF;		//法線
	sphere->AddCollisionInfo(infoSphere);	//衝突情報を追加

	CollisionInfo infoCapsule;				//衝突情報
	infoCapsule.opponent = sphere;			//衝突相手のコライダー
	infoCapsule.contactPoint = contactP;	//衝突点
	infoCapsule.contactNormal =				//反転法線
	{
		-normalF.x,
		-normalF.y,
		-normalF.z
	};
	capsule->AddCollisionInfo(infoCapsule);	//衝突情報を追加
}