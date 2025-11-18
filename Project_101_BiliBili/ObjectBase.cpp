#include "ObjectBase.h"
//#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include "Engine.h"
#include "Renderer.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace RenderData;
using namespace MeshData;

//コンストラクタ
ObjectBase::ObjectBase(
	MESH_TYPE meshType,
	XMFLOAT3 position,
	XMFLOAT3 rotation,
	XMFLOAT3 scale,
	XMFLOAT3 velocity,
	bool isActive, 
	ColliderType colliderType, 
	XMFLOAT3 collisionBoxSize, 
	bool collisionIsTrigger
)
	: m_meshType(meshType), m_position(position), m_rotation(rotation), m_scale(scale), m_velocity(velocity), m_isActive(isActive)
{
	//コライダーの生成
	m_pCollider = new Collider(
		this,				//所有者オブジェクト
		colliderType,		//コライダータイプ
		collisionBoxSize,	//コライダーのボックスサイズ
		collisionIsTrigger	//トリガーフラグ
	);
}

//デストラクタ
ObjectBase::~ObjectBase()
{
	//コライダーの破棄
	if (m_pCollider != nullptr)
	{
		delete m_pCollider;
		m_pCollider = nullptr;
	}
}

void ObjectBase::Update()
{
	UpdateOverride();
	m_pCollider->Update();
}

//衝突解決
void ObjectBase::ResolveCollisions()
{
	ResolveCollisionsOverride();	//衝突解決(固有処理用、派生クラスでオーバーライド)
	ClearCollisionInfos();			//衝突情報のクリア
}

//衝突情報の追加
void Collider::AddCollisionInfo(const CollisionInfo& info)
{
	m_collisionInfos.push_back(info);
}

//衝突情報のクリア
void Collider::ClearInfos()
{
	m_collisionInfos.clear();
}

//衝突情報のクリア
void ObjectBase::ClearCollisionInfos()
{
	m_pCollider->ClearInfos();
}

//ワールド行列の取得
const DirectX::XMMATRIX ObjectBase::GetWorldMatrix() const
{
	XMMATRIX T = XMMatrixTranslation(m_position.x, m_position.y, m_position.z);
	XMMATRIX R = XMMatrixRotationRollPitchYaw(
		XMConvertToRadians(m_rotation.x),
		XMConvertToRadians(m_rotation.y),
		XMConvertToRadians(m_rotation.z));
	XMMATRIX S = XMMatrixScaling(m_scale.x, m_scale.y, m_scale.z);
	return S * R * T;
}

//位置の取得
const DirectX::XMFLOAT3 ObjectBase::GetPosition() const
{
	return m_position;
}

//回転の取得
const DirectX::XMFLOAT3 ObjectBase::GetRotation() const
{
	return m_rotation;
}

//スケールの取得
const DirectX::XMFLOAT3 ObjectBase::GetScale() const
{
	return m_scale;
}

//色RGBAの取得
const DirectX::XMFLOAT4 ObjectBase::GetColor() const
{
	return m_color;
}

//アクティブかどうかを取得
const bool ObjectBase::IsActive() const
{
	return m_isActive;
}

//描画フラグの取得
const bool ObjectBase::IsDrawn() const
{
	return m_isDrawn;
}

//位置の設定
void ObjectBase::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
}

//回転の設定
void ObjectBase::SetRotation(DirectX::XMFLOAT3 rotation)
{
	m_rotation = rotation;
}

//スケールの設定
void ObjectBase::SetScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
}

//色RGBAの設定
void ObjectBase::SetColor(DirectX::XMFLOAT4 color)
{
	m_color = color;
}

//アクティブフラグの設定
void ObjectBase::SetActive(bool isActive)
{
	m_isActive = isActive;
}

//コライダーの取得
Collider* ObjectBase::GetCollider() const
{
	return m_pCollider;
}

//メッシュタイプの取得
MeshData::MESH_TYPE ObjectBase::GetMeshType() const
{
	return m_meshType;
}