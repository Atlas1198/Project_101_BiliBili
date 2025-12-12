#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "BB.h"
#include "ObjectManagerBase.h"
#include "SharedStruct.h"
#include "FrameTimer.h"

class Player;
class GameUIManager;
class CollisionManager;

//BBマネージャークラス
class BBManager : public ObjectManagerBase
{
public:
	static constexpr int BB_NUM = 2; //BBの数
	const wchar_t* lineBBTexPath = L"asset/texture/line.png";
	const wchar_t* electricityBBTexPath = L"asset/texture/white.png";

	inline static float BB_DURATION = 5.0f;

public:
	BBManager();		//コンストラクタ
	~BBManager();	//デストラクタ
	//メイン処理関数
	void InitializeOverride(	//初期化
		InputManager* pInputManager,
		TextureManager& textureManager,
		MeshManager& meshManager,
		CollisionManager& collisionManager
	) override;
	void UpdateOverride() override;					//更新
	void SubmitDrawsOverride(Renderer& renderer) override;		//描画要求提出
	void ResolveCollisionsOverride() override;		//衝突解決
	void FinalizeOverride() override;					//終了

	void SetPlayerData(std::vector<Player*>& players);				//プレイヤー情報の設定
	void SetGameUIManager(GameUIManager* pUIManager);				//UIマネージャーの設定
	void SetCollisionManager(CollisionManager* pCollisionManager);	//衝突マネージャーの設定
	void SetBB(int teamID, bool activate);						//BBの発動・無効化設定
	void OnItemPickup(int teamID);								//アイテム取得コールバック

private:
	BB* m_BB[BB_NUM] = { nullptr }; //BB配列
	std::vector<RenderData::RenderInfo> m_LineBBInfo;			//BBライン描画情報
	std::vector<RenderData::RenderInfo> m_ElectricityBBInfo;	//BB電気描画情報

	float m_BBTimer[BB_NUM] = { 0.0f }; //BBタイマー
	FrameTimer m_frameTimer[BB_NUM];



	GameUIManager* m_pUIManager = nullptr; //UIマネージャーへのポインタ
	CollisionManager* m_pCollisionManager = nullptr; //衝突マネージャーへのポインタ
private:
	void PrepareRenderInfo(	//BB描画情報生成
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
};