#pragma once
#define WIN32_LEAN_AND_MEAN
#include <d3d12.h>
#include <DirectXMath.h>
#include "SharedStruct.h"

//前方宣言
class ObjectBase;

//コライダータイプ列挙型
enum class ColliderType
{
	BOX,		//ボックス
	SPHERE,		//球
	CAPSULE,	//カプセル
	NONE		//なし
};

//ボックスコライダー構造体
struct BoxCollider
{
	DirectX::XMFLOAT3 center;		//中心点
	DirectX::XMFLOAT3 scale;		//サイズ（幅、高さ、奥行き）
	DirectX::XMFLOAT3 defaultScale;	//初期サイズ
};

//球コライダー構造体
struct SphereCollider
{
	DirectX::XMFLOAT3 center;	//中心点
	float radius;				//半径
	float defaultRadius;		//初期半径
};

//カプセルコライダー構造体
struct CapsuleCollider
{
	DirectX::XMFLOAT3 pointA;	//端点A(底面中心)
	DirectX::XMFLOAT3 pointB;	//端点B(頂点中心)
	float cylHeight;				//端点間の高さ
	float defaultHeight;		//初期高さ
	float radius;				//半径
	float defaultRadius;		//初期半径
};

//軸平行境界ボックス構造体
struct AABB
{
	DirectX::XMFLOAT3 min;	//最小座標
	DirectX::XMFLOAT3 max;	//最大座標
};

//衝突判定用の矩形クラス
class Collider
{
public:
	Collider(		//コンストラクタ
		ObjectBase* owner,						//所有者オブジェクト
		ColliderType type,						//コライダータイプ
		DirectX::XMFLOAT3 boxSize =
		DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),	//ボックスサイズ
		bool isTrigger = false					//トリガーフラグ
	);
	~Collider();	//デストラクタ

	void Update();			//コライダー更新
	void UpdateCollider();	//各種コライダー更新
	void UpdateAABB();		//AABB更新

	void AddCollisionInfo(const CollisionData::CollisionInfo& info);	//衝突情報追加
	void ClearInfos();									//衝突情報配列クリア

	//ゲッター
	ObjectBase* GetOwner() const;												//所有者オブジェクト取得
	ColliderType GetType() const;												//コライダータイプ取得
	const bool IsTrigger() const;												//トリガーフラグ取得
	const AABB GetAABB();														//軸平行境界ボックス取得
	const BoxCollider GetBoxCollider();											//ボックスコライダー取得
	const SphereCollider GetSphereCollider();									//球コライダー取得
	const CapsuleCollider GetCapsuleCollider();									//カプセルコライダー取得
	const DirectX::XMMATRIX GetWorldMatrix() const;								//ワールド行列の取得
	const std::vector<CollisionData::CollisionInfo>& GetCollisionInfos() const;	//衝突情報配列取得
	const bool isDetected() const;												//衝突検知フラグ取得
	const bool deleteFlag() const;												//デリートフラグ
	DirectX::XMFLOAT3 GetCenter() const;										//中心座標取得
	DirectX::XMFLOAT3 GetScale() const;											//サイズ取得
	DirectX::XMFLOAT3 GetRotation() const;										//回転取得

	//セッター
	void SetDetected(bool flag);	//衝突検知フラグ
	void SetDeleteFlag(bool flag);	//デリートフラグ

private:
	ObjectBase* m_pOwner = nullptr;	//所有者オブジェクト
	ColliderType m_type;			//コライダータイプ
	bool m_isTrigger = false;		//トリガーフラグ(物理衝突を無視するかどうか)

	AABB m_aabb;	//軸平行境界ボックス(BroadPhase用)

	//各種コライダー(テスト用に全て保持)
	BoxCollider m_boxCollider;			//ボックスコライダー
	SphereCollider m_sphereCollider;	//球コライダー
	CapsuleCollider m_capsuleCollider;	//カプセルコライダー

	//ワールド情報
	DirectX::XMFLOAT3 m_center;		//中心座標
	DirectX::XMFLOAT3 m_scale;		//サイズ
	DirectX::XMFLOAT3 m_rotation;	//回転
	DirectX::XMFLOAT3 m_scaleOffset;	//オブジェクトとのサイズ差

	std::vector<CollisionData::CollisionInfo> m_collisionInfos; //衝突情報配列(所有者オブジェクト用)

	bool m_isDetected = false; //衝突検知フラグ（描画用）

	bool m_deleteFlag = false;	//デリートフラグ

private:
	//コライダー生成関数
	void CreateCollider(DirectX::XMFLOAT3 scale);			//コライダー生成
	void CreateBoxCollider(DirectX::XMFLOAT3 scale);		//ボックスコライダー生成
	void CreateSphereCollider(DirectX::XMFLOAT3 scale);		//球コライダー生成
	void CreateCapsuleCollider(DirectX::XMFLOAT3 scale);	//カプセルコライダー生成

	//コライダー更新関数
	void UpdateBoxCollider();		//ボックスコライダー更新
	void UpdateSphereCollider();	//球コライダー更新
	void UpdateCapsuleCollider();	//カプセルコライダー更新

	//AABB更新関数
	void UpdateAABBBox();			//AABB更新(ボックスコライダー用)
	void UpdateAABBSphere();		//AABB更新(球コライダー用)
	void UpdateAABBCapsule();		//AABB更新(カプセルコライダー用)
};