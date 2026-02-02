#pragma once
#include "AssimpLoader.h"
#include "ObjectBase.h"
#include "RenderData.h"
#include "Context.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;
class CollisionManager;
class Collider;

//オブジェクト管理クラス
//全オブジェクトの管理を行うクラスの基底クラス
class ObjectManagerBase
{
public:
	ObjectManagerBase() {};		//コンストラクタ
	~ObjectManagerBase() {};	//デストラクタ

	void Initialize(	//初期化
		SceneContext* sceneContext,		//シーンコンテキスト構造体の参照
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	);
	void Update();							//更新
	void SubmitDraws(Renderer& renderer);	//描画要求をシーンに提出
	void ResolveCollisions();				//衝突後処理
	void Finalize();						//終了

protected:	//非公開メンバ変数
	SceneContext* m_pSceneContext = nullptr;	//シーンコンテキスト構造体

protected:
	virtual void InitializeOverride(	//初期化
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) = 0;
	virtual void UpdateOverride() = 0;							//シーン固有の更新
	virtual void SubmitDrawsOverride(Renderer& renderer) = 0;	//シーン固有の描画要求提出
	virtual void ResolveCollisionsOverride() = 0;				//シーン固有の衝突後処理
	virtual void FinalizeOverride() = 0;						//シーン固有の終了

	virtual void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) = 0;

	void SubmitRenderInfo(	//描画情報をシーンに提出
		Renderer& renderer,							//シーンの参照
		ObjectBase& objects,					//ゲームオブジェクト
		std::vector <WorldRenderInfo>& info	//描画情報構造体
	);

	void SubmitColliders(	//コライダー描画要求をシーンに提出
		CollisionManager& collisionManager,	//衝突管理クラスの参照
		Collider* pCollider					//コライダー配列のポインタ
	);
};