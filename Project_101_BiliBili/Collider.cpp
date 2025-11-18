#include "Collider.h"
#include "ObjectBase.h"

using namespace DirectX;

//コンストラクタ
Collider::Collider(ObjectBase* owner, ColliderType type, XMFLOAT3 scale, bool isTrigger)
	: m_pOwner(owner), m_isTrigger(isTrigger), m_type(type)
{
	CreateCollider(scale);
	UpdateCollider();	//コライダー初期化
	UpdateAABB();		//軸平行境界ボックス初期化
}

//デストラクタ
Collider::~Collider()
{
}

//更新
void Collider::Update()
{
	UpdateCollider();
	UpdateAABB();
}

//コライダー更新
void Collider::UpdateCollider()
{
	switch (m_type)
	{
	case ColliderType::BOX:
		UpdateBoxCollider();
		break;
	case ColliderType::SPHERE:
		UpdateSphereCollider();
		break;
	case ColliderType::CAPSULE:
		UpdateCapsuleCollider();
		break;
	default:
		break;
	}
}

//軸平行境界ボックス更新
void Collider::UpdateAABB()
{
	switch (m_type)
	{
	case ColliderType::BOX:
		UpdateAABBBox();
		break;
	case ColliderType::SPHERE:
		UpdateAABBSphere();
		break;
	case ColliderType::CAPSULE:
		UpdateAABBCapsule();
		break;
	default:
		break;
	}
}

//所有者オブジェクト取得
ObjectBase* Collider::GetOwner() const
{
	return m_pOwner;
}

//コライダータイプ取得
ColliderType Collider::GetType() const
{
	return m_type;
}

//トリガーフラグ取得
const bool Collider::IsTrigger() const
{
	return m_isTrigger;
}

//軸平行境界ボックス取得
const AABB Collider::GetAABB() 
{
	return m_aabb;
}

//ボックスコライダー取得
const BoxCollider Collider::GetBoxCollider()
{
	return m_boxCollider;
}

//球コライダー取得
const SphereCollider Collider::GetSphereCollider()
{
	return m_sphereCollider;
}

//カプセルコライダー取得
const CapsuleCollider Collider::GetCapsuleCollider()
{
	return m_capsuleCollider;
}

//ワールド行列の取得
const DirectX::XMMATRIX Collider::GetWorldMatrix() const
{
	XMMATRIX T = XMMatrixTranslation(m_center.x, m_center.y, m_center.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));
	XMMATRIX S = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	return S * R * T;
}

//衝突情報配列取得
const std::vector<CollisionInfo>& Collider::GetCollisionInfos() const
{
	return m_collisionInfos;
}

//衝突検知フラグ取得
const bool Collider::isDetected() const
{
	return m_isDetected;
}

//中心座標取得
DirectX::XMFLOAT3 Collider::GetCenter() const
{
	return m_center;
}

//サイズ取得
DirectX::XMFLOAT3 Collider::GetScale() const
{
	return m_scale;
}

//回転取得
DirectX::XMFLOAT3 Collider::GetRotation() const
{
	return m_rotation;
}

//衝突検知フラグ設定
void Collider::setDetected(bool flag)
{
	m_isDetected = flag;
}

//コライダー生成関数
void Collider::CreateCollider(DirectX::XMFLOAT3 scale)
{
	switch (m_type)
	{
	case ColliderType::BOX:
		CreateBoxCollider(scale);
		break;
	case ColliderType::SPHERE:
		CreateSphereCollider(scale);
		break;
	case ColliderType::CAPSULE:
		CreateCapsuleCollider(scale);
		break;
	default:
		break;
	}
}

//コライダー生成関数
//ボックスコライダー生成
void Collider::CreateBoxCollider(XMFLOAT3 scale)
{
	m_boxCollider.scale = scale;			//ボックスコライダーサイズ初期化
	m_boxCollider.defaultScale = scale;	//ボックスコライダー初期サイズ保存

	const XMFLOAT3 ownerScale = m_pOwner->GetScale();

	m_scaleOffset =
	{
		 m_boxCollider.scale.x - ownerScale.x,
		 m_boxCollider.scale.y - ownerScale.y,
		 m_boxCollider.scale.z - ownerScale.z,
	};
}

//球コライダー生成
void Collider::CreateSphereCollider(XMFLOAT3 scale)
{
	//球コライダースケール
	const float maxScale = 
		(std::max)(scale.x, (std::max)(scale.y, scale.z));	//各軸のスケールから一番大きいものを取得
	const float diamiter = maxScale;					//最大値を直径として使用
	const float radius = diamiter * 0.5f;				//半径

	m_sphereCollider.radius = radius;			//球コライダー半径初期化
	m_sphereCollider.defaultRadius = radius;	//球コライダー初期半径保存

	//コライダースケールの反映
	m_scale =
	{
		diamiter,
		diamiter,
		diamiter
	};

	//スケールオフセットの設定
	const XMFLOAT3 ownerScale = m_pOwner->GetScale();	//オーナーオブジェクトのサイズを取得
	const float ownerScaleMax = 
		(std::max)(ownerScale.x, (std::max)(ownerScale.y, ownerScale.z));	//各軸のスケールから一番大きいものを取得

	//オーナーオブジェクトのスケール最大値と直径の差をオフセット
	m_scaleOffset =
	{
		diamiter - ownerScaleMax,
		diamiter - ownerScaleMax,
		diamiter - ownerScaleMax
	};
}

//カプセルコライダー生成
void Collider::CreateCapsuleCollider(XMFLOAT3 scale)
{
	//サイズ取得
	const XMFLOAT3 ownerScale = m_pOwner->GetScale();
	const float sx = scale.x * ownerScale.x;
	const float sy = scale.y * ownerScale.y;
	const float sz = scale.z * ownerScale.z;

	//カプセルコライダーサイズ計算
	const float diamiter = (std::max)(sx, sz);			//直径(水平方向の最大値)
	const float radius = diamiter / 2.0f;				//半径
	const float capusleHeight = sy;						//カプセル高さ
	const float cylHeight = 
		(std::max)(0.0f, capusleHeight - diamiter);		//円柱部分の高さ(負の値にならないようにする)

	//カプセルサイズ初期化
	m_capsuleCollider.radius = radius;					//カプセルコライダー半径初期化
	m_capsuleCollider.defaultRadius = radius;			//カプセルコライダー初期半径保存
	m_capsuleCollider.cylHeight = cylHeight;			//カプセルコライダー高さ初期化
	m_capsuleCollider.defaultHeight = cylHeight;		//カプセルコライダー初期高さ保存

	//コライダーサイズ初期化
	m_scale =
	{
		diamiter,
		capusleHeight,
		diamiter
	};

	//オブジェクトとのサイズ差計算
	const float ownerSx = m_pOwner->GetScale().x;
	const float ownerSy = m_pOwner->GetScale().y;
	const float ownerSz = m_pOwner->GetScale().z;

	const float maxHorizontal = (std::max)(ownerSx, ownerSz);

	m_scaleOffset =
	{
		diamiter - maxHorizontal,
		m_scale.y - ownerSy,
		diamiter - maxHorizontal
	};
}

//コライダー更新関数
void Collider::UpdateBoxCollider()
{
	//中心点更新
	XMFLOAT3 position = m_pOwner->GetPosition();	//オーナーオブジェクトの位置取得
	m_boxCollider.center = position;				//ボックスコライダー中心点更新
	m_center = position;							//コライダーの中心座標

	//スケール反映
	XMFLOAT3 ownerScale = m_pOwner->GetScale();		//オーナーオブジェクトのスケール取得
	m_boxCollider.scale =
	{
		ownerScale.x + m_scaleOffset.x,
		ownerScale.y + m_scaleOffset.y,
		ownerScale.z + m_scaleOffset.z
	};

	//コライダーサイズ更新
	m_scale = m_boxCollider.scale;

	//回転反映
	m_rotation = m_pOwner->GetRotation();
}

//球コライダー更新
void Collider::UpdateSphereCollider()
{
	//中心点更新
	XMFLOAT3 position = m_pOwner->GetPosition();	//オーナーオブジェクトの位置取得
	m_center = position;							//コライダーの中心座標

	//スケール反映
	XMFLOAT3 scale = m_pOwner->GetScale();			//オーナーオブジェクトのスケール取得
	const float sx = scale.x;
	const float sy = scale.y;
	const float sz = scale.z;

	const float offsetScaleX = sx + m_scaleOffset.x;
	const float offsetScaleY = sy + m_scaleOffset.y;
	const float offsetScaleZ = sz + m_scaleOffset.z;

	const float diamiter = 
		(std::max)(offsetScaleX, (std::max)(offsetScaleY, offsetScaleZ));	//直径(水平方向の最大値)
	const float radius = diamiter / 2.0f;									//半径

	//球コライダーサイズ更新
	m_sphereCollider.radius = radius;				//球コライダー半径更新

	//コライダーサイズ更新
	m_scale =
	{
		diamiter,
		diamiter,
		diamiter
	};
}

//カプセルコライダー更新
void Collider::UpdateCapsuleCollider()
{
	//中心点更新
	XMFLOAT3 position = m_pOwner->GetPosition();	//オーナーオブジェクトの位置取得
	m_center = position;							//コライダーの中心座標

	//スケール反映
	const XMFLOAT3 ownerScale = m_pOwner->GetScale();	//オーナーオブジェクトのスケール取得
	const float sx = ownerScale.x;
	const float sy = ownerScale.y;
	const float sz = ownerScale.z;

	const float diamiter = 
		(std::max)(sx + m_scaleOffset.x, sz + m_scaleOffset.z);	//直径(水平方向の最大値)
	const float radius = diamiter / 2.0f;						//半径
	const float capusleHeight = sy + m_scaleOffset.y;			//カプセル高さ
	const float cylHeight = 
		(std::max)(0.0f, capusleHeight - diamiter);				//円柱部分の高さ(負の値にならないようにする)

	//カプセルサイズ更新
	m_capsuleCollider.radius = radius;					//カプセルコライダー半径更新
	m_capsuleCollider.cylHeight = cylHeight;				//カプセルコライダー高さ更新

	//回転反映
	m_rotation = m_pOwner->GetRotation();

	//ローカル軸方向ベクトル取得
	XMVECTOR dirLocal = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f); //Y軸方向ベクトル

	//回転行列取得
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));

	//ワールド軸方向ベクトル計算
	XMVECTOR axisWorld = XMVector3Normalize(XMVector3TransformNormal(dirLocal, R));

	//端点A,B計算
	float halfHeight = cylHeight * 0.5f;					//シリンダーの半分の高さ
	XMVECTOR center = XMLoadFloat3(&m_center);				//中心点
	XMVECTOR offset = XMVectorScale(axisWorld, halfHeight);	//オフセットベクトル

	XMVECTOR pA = XMVectorSubtract(center, offset);	//端点A
	XMVECTOR pB = XMVectorAdd(center, offset);		//端点B

	XMStoreFloat3(&m_capsuleCollider.pointA, pA);	//端点A保存
	XMStoreFloat3(&m_capsuleCollider.pointB, pB);	//端点B保存

	//コライダーサイズ更新
	m_scale =
	{
		diamiter,
		capusleHeight,
		diamiter
	};
}

//軸平行境界ボックス更新(ボックスコライダー用)
void Collider::UpdateAABBBox()
{
	//ボックスコライダー情報取得
	const XMFLOAT3 center = m_center;
	const XMFLOAT3 scale = m_boxCollider.scale;
	const float halfX = scale.x * 0.5f;
	const float halfY = scale.y * 0.5f;
	const float halfZ = scale.z * 0.5f;

	//回転行列取得
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));

	//各軸方向ベクトル取得
	XMVECTOR u0 = XMVector3Normalize(R.r[0]); //X軸方向ベクトル
	XMVECTOR u1 = XMVector3Normalize(R.r[1]); //Y軸方向ベクトル
	XMVECTOR u2 = XMVector3Normalize(R.r[2]); //Z軸方向ベクトル

	//AABB半分のサイズ計算
	float aabbHalfX =
		fabsf(XMVectorGetX(u0)) * halfX +
		fabsf(XMVectorGetX(u1)) * halfY +
		fabsf(XMVectorGetX(u2)) * halfZ;
	float aabbHalfY =
		fabsf(XMVectorGetY(u0)) * halfX +
		fabsf(XMVectorGetY(u1)) * halfY +
		fabsf(XMVectorGetY(u2)) * halfZ;
	float aabbHalfZ =
		fabsf(XMVectorGetZ(u0)) * halfX +
		fabsf(XMVectorGetZ(u1)) * halfY +
		fabsf(XMVectorGetZ(u2)) * halfZ;

	//AABB更新
	m_aabb.min = {
		center.x - aabbHalfX,
		center.y - aabbHalfY,
		center.z - aabbHalfZ
	};
	m_aabb.max = {
		center.x + aabbHalfX,
		center.y + aabbHalfY,
		center.z + aabbHalfZ
	};
}

//軸平行境界ボックス更新(球コライダー用)
void Collider::UpdateAABBSphere()
{
	const float radius = m_sphereCollider.radius;
	const XMFLOAT3 center = m_center;

	m_aabb.min = {
	center.x - radius,
	center.y - radius,
	center.z - radius
	};

	m_aabb.max = {
	center.x + radius,
	center.y + radius,
	center.z + radius
	};
}

//軸平行境界ボックス更新(カプセルコライダー用)
void Collider::UpdateAABBCapsule()
{
	const float radius = m_capsuleCollider.radius;
	const XMFLOAT3 pointA = m_capsuleCollider.pointA;
	const XMFLOAT3 pointB = m_capsuleCollider.pointB;
	m_aabb.min = {
		(std::min)(pointA.x, pointB.x) - radius,
		(std::min)(pointA.y, pointB.y) - radius,
		(std::min)(pointA.z, pointB.z) - radius
	};
	m_aabb.max = {
		(std::max)(pointA.x, pointB.x) + radius,
		(std::max)(pointA.y, pointB.y) + radius,
		(std::max)(pointA.z, pointB.z) + radius
	};
}