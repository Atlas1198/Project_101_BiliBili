#pragma once
#include "AssimpLoader.h"
#include "ObjectBase.h"

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
		InputManager* pInputManager,		//入力マネージャーのポインタ
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	);
	virtual void Update() = 0;							//更新
	virtual void SubmitDraws(Renderer& renderer) = 0;	//描画要求をシーンに提出
	virtual void ResolveCollisions() = 0;				//衝突後処理

protected:
	virtual void InitializeOverride(	//初期化
		InputManager* pInputManager,		//入力マネージャーのポインタ
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) = 0;

	virtual void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) = 0;

	void SubmitRenderInfo(	//描画情報をシーンに提出
		Renderer& renderer,							//シーンの参照
		const ObjectBase& objects,					//ゲームオブジェクト
		std::vector <RenderData::RenderInfo>& info	//描画情報構造体
	);

	void SubmitColliders(	//コライダー描画要求をシーンに提出
		CollisionManager& collisionManager,	//衝突管理クラスの参照
		Collider* pCollider					//コライダー配列のポインタ
	);
};