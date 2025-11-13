#pragma once
#include "AssimpLoader.h"
#include "GameObject.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;
class CollisionManager;
class Collider;

//ゲームオブジェクト管理クラス
//全ゲームオブジェクトの管理を行うクラスの基底クラス
class GameObjectManager
{
public:
	GameObjectManager() {};		//コンストラクタ
	~GameObjectManager() {};	//デストラクタ

	virtual void Update() = 0;				//更新
	virtual void ResolveCollisions() = 0;	//衝突後処理

protected:
	virtual void Initialize(	//初期化
		InputManager* pInputManager,		//入力マネージャーのポインタ
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) = 0;

	virtual void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) = 0;
	
	void SubmitDraws(	//描画要求をシーンに提出
		Renderer& renderer,									//シーンの参照
		const GameObject& objects,							//ゲームオブジェクト
		const std::vector <RenderData::RenderInfo>& info	//描画情報構造体
	);

	void SubmitColliders(	//コライダー描画要求をシーンに提出
		CollisionManager& collisionManager,	//衝突管理クラスの参照
		Collider* pCollider					//コライダー配列のポインタ
	);
};