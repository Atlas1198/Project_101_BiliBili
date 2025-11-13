#include "Collider.h"
#include "GameObject.h"

using namespace DirectX;

//コンストラクタ
Collider::Collider(GameObject* owner, ColliderType type, XMFLOAT3 size, bool isTrigger)
	: m_pOwner(owner), m_isTrigger(isTrigger), m_type(type)
{
	//switch (m_type)
	//{
	//case ColliderType::BOX:
	//	CreateBoxCollider(size);
	//	break;
	//case ColliderType::SPHERE:
	//	CreateSphereCollider(size.x);
	//	break;
	//case ColliderType::CAPSULE:
	//	CreateCapsuleCollider(size.x, size.y - size.x * 2.0f);
	//	break;
	//default:
	//	break;
	//}

	CreateBoxCollider(size);
	CreateSphereCollider(size.x);
	CreateCapsuleCollider(size.x, size.y - size.x * 2.0f);

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
	//switch (m_type)
	//{
	//case ColliderType::BOX:
	//	UpdateBoxCollider();
	//	break;
	//case ColliderType::SPHERE:
	//	UpdateSphereCollider();
	//	break;
	//case ColliderType::CAPSULE:
	//	UpdateCapsuleCollider();
	//	break;
	//default:
	//	break;
	//}

	UpdateBoxCollider();
	//UpdateSphereCollider();
	//UpdateCapsuleCollider();
}

//軸平行境界ボックス更新
void Collider::UpdateAABB()
{
	//半サイズ計算
	XMFLOAT3 halfSize = 
	{
		m_boxCollider.size.x / 2.0f,
		m_boxCollider.size.y / 2.0f,
		m_boxCollider.size.z / 2.0f
	};

	//最小座標
	m_aabb.min = 
	{
		m_boxCollider.center.x - halfSize.x,
		m_boxCollider.center.y - halfSize.y,
		m_boxCollider.center.z - halfSize.z
	};

	//最大座標
	m_aabb.max = 
	{
		m_boxCollider.center.x + halfSize.x,
		m_boxCollider.center.y + halfSize.y,
		m_boxCollider.center.z + halfSize.z
	};
}

//所有者オブジェクト取得
GameObject* Collider::GetOwner() const
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

//ワールド行列の取得
const DirectX::XMMATRIX Collider::GetWorldMatrix() const
{
	XMMATRIX T = XMMatrixTranslation(m_center.x, m_center.y, m_center.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(0),
		XMConvertToRadians(0),
		XMConvertToRadians(0));
	XMMATRIX S = XMMatrixScaling(m_size.x, m_size.y, m_size.z);
	return S * R * T;
}

//衝突検知フラグ取得
const bool Collider::isDetected() const
{
	return m_isDetected;
}

//衝突検知フラグ設定
void Collider::setDetected(bool flag)
{
	m_isDetected = flag;
}

//コライダー生成関数
//ボックスコライダー生成
void Collider::CreateBoxCollider(DirectX::XMFLOAT3 boxSize)
{
	m_boxCollider.size = boxSize;			//ボックスコライダーサイズ初期化
	m_boxCollider.defaultSize = boxSize;	//ボックスコライダー初期サイズ保存
}

//球コライダー生成
void Collider::CreateSphereCollider(float radius)
{
	m_sphereCollider.radius = radius;			//球コライダー半径初期化
	m_sphereCollider.defaultRadius = radius;	//球コライダー初期半径保存
}

//カプセルコライダー生成
void Collider::CreateCapsuleCollider(float radius, float height)
{
	m_capsuleCollider.radius = radius;				//カプセルコライダー半径初期化
	m_capsuleCollider.defaultRadius = radius;		//カプセルコライダー初期半径保存
	m_capsuleCollider.height = height;				//カプセルコライダー高さ初期化
	m_capsuleCollider.defaultHeight = height;		//カプセルコライダー初期高さ保存
}

//コライダー更新関数
void Collider::UpdateBoxCollider()
{
	//中心点更新
	XMFLOAT3 position = m_pOwner->GetPosition();	//オーナーオブジェクトの位置取得
	m_boxCollider.center = position;				//ボックスコライダー中心点更新
	m_center = position;							//コライダーの中心座標

	//スケール反映
	XMFLOAT3 scale = m_pOwner->GetScale();		//オーナーオブジェクトのスケール取得
	m_boxCollider.size =
	{
		m_boxCollider.defaultSize.x * scale.x,
		m_boxCollider.defaultSize.y * scale.y,
		m_boxCollider.defaultSize.z * scale.z
	};

	m_size = m_boxCollider.size;
}

//球コライダー更新
void Collider::UpdateSphereCollider()
{
	//中心点更新
	XMFLOAT3 position = m_pOwner->GetPosition();	//オーナーオブジェクトの位置取得
	m_sphereCollider.center = position;				//球コライダー中心点更新
	m_center = position;							//コライダーの中心座標
}

//カプセルコライダー更新
void Collider::UpdateCapsuleCollider()
{
	//中心点更新
	XMFLOAT3 position = m_pOwner->GetPosition();	//オーナーオブジェクトの位置取得
	m_center = position;							//コライダーの中心座標

	//スケール反映
	XMFLOAT3 scale = m_pOwner->GetScale();		//オーナーオブジェクトのスケール取得
	m_capsuleCollider.height = m_capsuleCollider.defaultHeight * scale.y;	//カプセルコライダー高さ更新
	
	m_capsuleCollider.pointA = 
	{//端点A更新
		m_center.x,
		m_center.y - (m_capsuleCollider.height / 2.0f),
		m_center.z
	};
	m_capsuleCollider.pointB = 
	{//端点B更新
		m_center.x,
		m_center.y + (m_capsuleCollider.height / 2.0f),
		m_center.z
	};

	m_capsuleCollider.radius = m_capsuleCollider.defaultRadius * scale.x;	//カプセルコライダー半径更新

	m_size = scale;
}
