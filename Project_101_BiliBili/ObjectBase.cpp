#include "ObjectBase.h"
//#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include "Engine.h"
#include "Renderer.h"
#include "CollisionManager.h"

using namespace DirectX;
using namespace CollisionData;

//コンストラクタ
ObjectBase::ObjectBase(
	MESH_TYPE meshType,
	XMFLOAT3 position,
	XMFLOAT3 rotation,
	XMFLOAT3 scale,
	XMFLOAT3 velocity,
	bool isActive, 
	OBJECT_TAG tag,
	COLLISION_LAYER layer,
	XMFLOAT3 colliderSetScale,
	XMFLOAT3 colliderSetOffsetPosition,
	XMFLOAT3 colliderSetOffsetScale,
	XMFLOAT3 colliderSetOffsetRotation
) : 
	m_meshType(meshType), 
	m_position(position), 
	m_rotation(rotation), 
	m_scale(scale), 
	m_velocity(velocity), 
	m_isActive(isActive), 
	m_tag(tag)
{
	m_pColliderSet = new ColliderSet(
		this,
		tag,
		m_position,
		colliderSetScale,
		m_rotation,
		layer,
		true,
		colliderSetOffsetPosition,
		colliderSetOffsetScale,
		colliderSetOffsetRotation
	);
}

//デストラクタ
ObjectBase::~ObjectBase()
{
	//コライダーの破棄
	if (m_pColliderSet)
	{
		m_pColliderSet->SetDeleteFlag(true);
		m_pColliderSet = nullptr;
	}
}

void ObjectBase::Update()
{
	UpdateOverride();
	m_pColliderSet->Update();
	UpdateAnimation();
}

//衝突解決
void ObjectBase::ResolveCollisions()
{
	m_pColliderSet->BuildObjectCollisionInfos(); //衝突情報を収集
	ResolveCollisionsOverride();	//衝突解決(固有処理用、派生クラスでオーバーライド)
	m_pColliderSet->Update();		//コライダーの更新
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
	m_pColliderSet->ClearCollisionInfos();
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

//描画フラグの設定
void ObjectBase::SetDrawn(bool isDrawn)
{
	m_isDrawn = isDrawn;
}

//アニメーション更新
void ObjectBase::UpdateAnimation()
{
	if (m_texSplitInfo.total <= 1 || m_texSplitInfo.updateRate <= 0) return;

	m_texSplitInfo.frameCount++;	//フレームカウントをインクリメント

	//更新頻度に達したらインデックスを更新
	if (m_texSplitInfo.frameCount >= m_texSplitInfo.updateRate)
	{
		m_texSplitInfo.frameCount = 0;	//フレームカウントリセット
		m_texSplitInfo.index++;			//インデックスをインクリメント

		//インデックスが総数を超えたらリセット
		if (m_texSplitInfo.index >= m_texSplitInfo.total)
		{
			m_texSplitInfo.index = 0;
		}
	}
}

//コライダーの取得
ColliderSet* ObjectBase::GetColliderSet() const
{
	return m_pColliderSet;
}

//メッシュタイプの取得
MESH_TYPE ObjectBase::GetMeshType() const
{
	return m_meshType;
}

//オブジェクトタグの取得
OBJECT_TAG ObjectBase::GetTag() const
{
	return m_tag;
}

//テクスチャ分割情報構造体取得関数
const TexSplitInfo& ObjectBase::GetTexSplitInfo() const
{
	return m_texSplitInfo;
}
