#pragma once
#include <d3d12.h>
#include <DirectXMath.h>

//前方宣言
class GameObject;

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
	DirectX::XMFLOAT3 size;			//サイズ（幅、高さ、奥行き）
	DirectX::XMFLOAT3 defaultSize;	//初期サイズ
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
	float height;				//端点間の高さ
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
	Collider(
		GameObject* owner,
		ColliderType type,
		DirectX::XMFLOAT3 boxSize = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f),
		bool isTrigger = false
	);
	~Collider();	//デストラクタ

	void Update();
	void UpdateCollider();
	void UpdateAABB();

	//ゲッター
	GameObject* GetOwner() const;					//所有者オブジェクト取得
	ColliderType GetType() const;					//コライダータイプ取得
	const bool IsTrigger() const;					//トリガーフラグ取得
	const AABB GetAABB();							//軸平行境界ボックス取得
	const BoxCollider GetBoxCollider();				//ボックスコライダー取得
	const DirectX::XMMATRIX GetWorldMatrix() const;	//ワールド行列の取得
	const bool isDetected() const;					//衝突検知フラグ取得

	//セッター
	void setDetected(bool flag);	//衝突検知フラグ設定

private:
	GameObject* m_pOwner = nullptr;	//所有者オブジェクト
	ColliderType m_type;			//コライダータイプ
	bool m_isTrigger = false;		//トリガーフラグ(物理衝突を無視するかどうか)

	AABB m_aabb;	//軸平行境界ボックス(BroadPhase用)

	//各種コライダー(テスト用に全て保持)
	BoxCollider m_boxCollider;			//ボックスコライダー
	SphereCollider m_sphereCollider;	//球コライダー
	CapsuleCollider m_capsuleCollider;	//カプセルコライダー

	DirectX::XMFLOAT3 m_center;	//中心座標
	DirectX::XMFLOAT3 m_size;	//サイズ

	bool m_isDetected = false; //衝突検知フラグ（描画用）

private:
	//コライダー生成関数
	void CreateBoxCollider(DirectX::XMFLOAT3 boxSize);		//ボックスコライダー生成
	void CreateSphereCollider(float radius);				//球コライダー生成
	void CreateCapsuleCollider(float radius, float height);	//カプセルコライダー生成

	//コライダー更新関数
	void UpdateBoxCollider();		//ボックスコライダー更新
	void UpdateSphereCollider();	//球コライダー更新
	void UpdateCapsuleCollider();	//カプセルコライダー更新
};