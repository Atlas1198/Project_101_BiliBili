#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include "ColliderSet.h"
#include "SharedStruct.h"
#include "RenderData.h"

// Node Animator Set Structure
struct NodeAnimatorSet
{
	NodeAnimatorSet() { pNodeAnimator = new NodeAnimator(); }

	bool isAnimLoaded = false;				// Animation loaded flag
	bool isAnimPlaying = false;				// Animation playing flag
	NodeAnimator* pNodeAnimator = nullptr;	// Pointer to node animator
};

//ゲームオブジェクトクラス
//全ゲームオブジェクトの基底クラス
class ObjectBase
{
public:	//公開関数
	ObjectBase(	//コンストラクタ
		MESH_TYPE meshType,								//メッシュタイプ
		DirectX::XMFLOAT3 position,						//座標
		DirectX::XMFLOAT3 rotation,						//回転
		DirectX::XMFLOAT3 scale,						//スケール
		DirectX::XMFLOAT3 velocity,						//移動速度
		bool isActive = true,							//アクティブフラグ
		OBJECT_TAG tag = OBJECT_TAG::NONE,				//オブジェクトタグ
		CollisionData::COLLISION_LAYER layer =
			CollisionData::COLLISION_LAYER::DEFAULT,	//衝突レイヤー
		DirectX::XMFLOAT3 colliderSetScale = 
			{1.0f, 1.0f, 1.0f},							//コライダーセットスケール
		DirectX::XMFLOAT3 colliderSetOffsetPosition = 
			{ 0.0f, 0.0f, 0.0f },						//コライダーセットオフセット位置
		DirectX::XMFLOAT3 colliderSetOffsetRotation = 
			{ 0.0f, 0.0f, 0.0f }						//コライダーセットオフセット回転
	);
	~ObjectBase();	//デストラクタ

	void Update();

	//衝突判定関連
	void ResolveCollisions();											//衝突解決
	void AddCollisionInfo(const CollisionData::CollisionInfo& info);	//衝突情報の追加
	void ClearCollisionInfos();											//衝突情報のクリア

	//ゲッター
	const DirectX::XMMATRIX GetWorldMatrix() const;	//ワールド行列の取得
	const DirectX::XMFLOAT3 GetPosition() const;	//位置の取得
	const DirectX::XMFLOAT3 GetRotation() const;	//回転の取得
	const DirectX::XMFLOAT3 GetScale() const;		//スケールの取得
	const DirectX::XMFLOAT4 GetColor() const;		//色RGBAの取得
	const bool IsActive() const;					//アクティブかどうかを取得
	const bool IsDrawn() const;						//描画フラグの取得
	ColliderSet* GetColliderSet() const;			//コライダーの取得
	MESH_TYPE GetMeshType() const;					//メッシュタイプの取得
	OBJECT_TAG GetTag() const;						//オブジェクトタグの取得
	const TexSplitInfo& GetTexSplitInfo() const;	//テクスチャ分割情報構造体取得関数
	NodeAnimatorSet* GetNodeAnimatorSet();			// Get node animation set

	//セッター
	void SetPosition(DirectX::XMFLOAT3 position);		//位置の設定
	void SetRotation(DirectX::XMFLOAT3 rotation);		//回転の設定
	void SetScale(DirectX::XMFLOAT3 scale);				//スケールの設定
	void SetColor(DirectX::XMFLOAT4 color);				//色RGBAの設定
	void SetActive(bool isActive);						//アクティブフラグの設定
	void SetDrawn(bool isDrawn);						//描画フラグの設定
	void SetTexSplitInfo(TexSplitInfo info);			//テクスチャ分割情報構造体の設定
	void SetSceneContext(SceneContext* pSceneContext);	//シーンコンテキスト構造体の設定
	void SetNodeAnimatorSet(const NodeAnimatorSet& nodeAnimatorSet);	// Set node animator set

protected:	//非公開メンバ変数
	DirectX::XMFLOAT3 m_position{ 0.0f, 0.0f, 0.0f };	//位置
	DirectX::XMFLOAT3 m_rotation{ 0.0f, 0.0f, 0.0f };	//回転
	DirectX::XMFLOAT3 m_scale{ 1.0f,1.0f,1.0f };		//スケール
	DirectX::XMFLOAT4 m_color{ 1.0f,1.0f,1.0f,1.0f };	//色RGBA
	DirectX::XMFLOAT3 m_velocity{ 0.0f, 0.0f, 0.0f };	//移動速度
	bool m_isActive = false;		//アクティブフラグ

	MESH_TYPE m_meshType = MESH_TYPE::QUAD;	//メッシュタイプ
	bool m_isDrawn = true;			//描画フラグ

	ColliderSet* m_pColliderSet = nullptr;	//コライダーセットへのポインタ
	OBJECT_TAG m_tag = OBJECT_TAG::NONE; //オブジェクトタグ
	TexSplitInfo m_texSplitInfo{}; //テクスチャ分割情報構造体

	SceneContext* m_pSceneContext = nullptr; //シーンコンテキスト構造体

	NodeAnimatorSet m_nodeAnimatorSet{}; // Node animator set

protected:	//非公開メンバ変数
	virtual void UpdateOverride() = 0;				//シーン固有の更新
	virtual void ResolveCollisionsOverride() = 0;	//シーン固有の衝突解決
	void UpdateAnimation();							//アニメーション更新
};