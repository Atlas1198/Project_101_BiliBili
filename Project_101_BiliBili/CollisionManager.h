#pragma once
#define WIN32_LEAN_AND_MEAN
#include <d3d12.h>
#include <DirectXMath.h>
#include <vector>
#include "Collider.h"
#include "SharedStruct.h"

//前方宣言
class Renderer;
class TextureManager;
class MeshManager;

//衝突ペア構造体
struct CollisionPair
{
	Collider* colliderA; // コライダーA
	Collider* colliderB; // コライダーB
};

// 衝突管理クラス
class CollisionManager
{
public:
	static constexpr DirectX::XMFLOAT4 DRAW_COLOR_DEFAULT = { 0.0f, 1.0f, 0.0f, 0.1f };		//描画時のデフォルトカラー
	static constexpr DirectX::XMFLOAT4 DRAW_COLOR_DETECTED = { 1.0f, 0.0f, 0.0f, 0.1f };	//描画時の衝突検知時カラー

public:
	const wchar_t* texPath = L"asset/texture/white.png";

public:
	CollisionManager();		//コンストラクタ
	~CollisionManager();	//デストラクタ

	//メイン関数
	void Initialize(
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	);
	void Draw(Renderer& renderer);		//描画
	void SubmitDraw(
		Renderer& renderer,									//シーンの参照
		const Collider& collider,							//コライダー配列
		const std::vector <RenderData::RenderInfo>& info	//描画情報構造体
	);

	//衝突判定処理
	void CheckCollisions(); //衝突判定
	void BroadPhase();    //ブロードフェーズ
	void NarrowPhase();  //ナローフェーズ

	//コライダー配列の操作
	void RegisterCollider(Collider* collider);	//コライダー登録
	void RemoveCollider(Collider* collider);		//コライダー削除
	void ClearColliders();								//コライダークリア

	void CreateCollisionInfo();	//衝突情報作成

	void CreateColliderRenderInfo(	//コライダー描画情報作成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	);

private:
	std::vector<Collider*> m_pCollidersList;				//コライダー配列
	std::vector<CollisionPair> m_pNarrowPhaseColliders;		//ナローフェーズ用コライダー配列
	std::vector<CollisionPair> m_previousCollisionPairs;	//前回の衝突ペア配列

	std::vector<RenderData::RenderInfo> m_colliderRenderInfoBox;		//ボックスコライダー描画情報
	std::vector<RenderData::RenderInfo> m_colliderRenderInfoSphere;		//球コライダー描画情報
	std::vector<RenderData::RenderInfo> m_colliderRenderInfoCapsule;	//カプセルコライダー描画情報
};