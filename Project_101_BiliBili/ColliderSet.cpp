#include "ColliderSet.h"
#include "CollisionManager.h"
#include "ObjectBase.h"

using namespace DirectX;

//コンストラクタ
ColliderSet::ColliderSet(
	ObjectBase* m_owner, 
	OBJECT_TAG ownerTag,
	DirectX::XMFLOAT3 basePosition, DirectX::XMFLOAT3 baseScale, DirectX::XMFLOAT3 baseRotation,
	CollisionData::COLLISION_LAYER layer,
	bool enabled,
	DirectX::XMFLOAT3 offsetPosition, DirectX::XMFLOAT3 offsetScale, DirectX::XMFLOAT3 offsetRotation,
	bool isTrigger
	)
	: 
	m_owner(m_owner), 
	m_ownerTag(ownerTag),
	m_basePosition(basePosition), m_baseScale(baseScale), m_baseRotation(baseRotation),
	m_layer(layer),
	m_isActive(enabled),
	m_offsetPosition(offsetPosition), m_offsetScale(offsetScale), m_offsetRotation(offsetRotation),
	m_isTrigger(isTrigger)
{
}

//デストラクタ
ColliderSet::~ColliderSet()
{
	for(auto& collider : m_colliders)
	{
		delete collider;
		collider = nullptr;
	}

	m_colliders.clear();
}

//更新
void ColliderSet::Update()
{
	if (!m_isActive) return;

	//オーナーの変換情報取得
	XMFLOAT3 ownerPosition = m_owner->GetPosition();
	XMFLOAT3 ownerScale = m_owner->GetScale();
	XMFLOAT3 ownerRotation = m_owner->GetRotation();

	//コライダーの変換更新
	for(auto& collider : m_colliders)
	{
		collider->Update(
			ownerPosition,
			ownerScale,
			ownerRotation
		);
	}
}

//コライダー提出
void ColliderSet::RegisterColliders(CollisionManager& collisionManager)
{
	if (!m_isActive) return;
	for (auto& collider : m_colliders)
	{
		collisionManager.RegisterCollider(collider);
	}
}

//コライダー追加
void ColliderSet::AddCollider(
	ColliderType type,							//コライダータイプ
	DirectX::XMFLOAT3 localCenter,				//ローカル中心座標
	DirectX::XMFLOAT3 localScale,				//ローカルスケール
	DirectX::XMFLOAT3 localRotation				//ローカル回転
)
{
	m_colliders.push_back(
		new Collider(
			this,
			localCenter,
			localScale,
			localRotation,
			type,
			m_ownerTag,
			m_layer,
			m_isTrigger
		)
	);
}

//コライダー配列取得
const std::vector<Collider*>& ColliderSet::GetColliders() const
{
	return m_colliders;
}

//コライダーの基準変換を更新
void ColliderSet::UpdateCollidersTransform()
{
}

//衝突情報を収集
void ColliderSet::CollectCollisionInfos()
{
}

//有効フラグ設定
void ColliderSet::SetActive(bool enabled)
{
	m_isActive = enabled;
	for(auto& collider : m_colliders)
	{
		collider->SetActive(enabled);
	}
}

//デリートフラグ設定
void ColliderSet::SetDeleteFlag(bool flag)
{
	for (auto& collider : m_colliders)
	{
		collider->SetDeleteFlag(flag);
	}
}

//衝突情報配列クリア
void ColliderSet::ClearCollisionInfos()
{
	m_collisionInfos.clear();
}
