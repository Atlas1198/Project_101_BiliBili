#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "ObjectManagerBase.h"
#include "SharedStruct.h"
#include "Item.h"
#include "FrameTimer.h"
#include "Context.h"
#include "Random.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

//フィールド管理クラス
class ItemManager : public ObjectManagerBase
{
	const wchar_t* itemTexPath = L"asset/texture/game_scene/change_item.png";
public:
	ItemManager();		//コンストラクタ
	~ItemManager();	//デストラクタ
	void SpawnItem();
	inline static float ITEM_RESPAWN = 15.0f;
	static constexpr float EVENT_SPAWN_RATE = 0.5f;
	const int MAX_SKIPS = 2;

private:	//非公開メンバ変数
	//メイン処理関数
	void InitializeOverride(	//初期化
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) override;
	void UpdateOverride() override;							//更新
	void SubmitDrawsOverride(Renderer& renderer) override;	//描画要求をシーンに提出
	void ResolveCollisionsOverride() override;				//衝突後処理
	void FinalizeOverride() override;						//終了
	void StartTimer() { m_frameTimer.Mark(); m_totalTimer.Mark(); }

	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

private:
	std::vector<Item*> m_pItems;					//アイテムオブジェクト配列
	std::vector<WorldRenderInfo> m_itemInfo;		//アイテム描画情報
	Random* m_pRandom = nullptr;					//乱数生成クラスのポインタ
	CollisionManager *m_pCollisionManager = nullptr; //衝突管理クラスのポインタ
	FrameTimer m_frameTimer;
	FrameTimer m_totalTimer;
	int nextItemIndex = 1;
	int skips[2] = { 1, 4 };
	bool applyNewSpawnRate = false;
	bool showedAnnouncement = false;
};