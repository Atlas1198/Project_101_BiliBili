#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "BB.h"
#include "ObjectManagerBase.h"
#include "SharedStruct.h"
#include "FrameTimer.h"
#include "RenderData.h"
#include "BilibiliArea.h"

class Player;
class GameUIManager;
class CollisionManager;

//BB繝槭ロ繝ｼ繧ｸ繝｣繝ｼ繧ｯ繝ｩ繧ｹ
class BBManager : public ObjectManagerBase
{
public:
	static constexpr int BB_NUM = 2; //BB縺ｮ謨ｰ
	static constexpr int BB_AREA_NUM = 4;
	const wchar_t* lineBBTexPath = L"asset/texture/line.png";
	const wchar_t* electricityBBTexPath = L"asset/texture/effect/bilibili.png";

	inline static float BB_DURATION = 5.0f;

public:
	BBManager();		//繧ｳ繝ｳ繧ｹ繝医Λ繧ｯ繧ｿ
	~BBManager();	//繝・せ繝医Λ繧ｯ繧ｿ
	//繝｡繧､繝ｳ蜃ｦ逅・未謨ｰ
	void InitializeOverride(	//蛻晄悄蛹・
		InputManager* pInputManager,
		TextureManager& textureManager,
		MeshManager& meshManager,
		CollisionManager& collisionManager
	) override;
	void UpdateOverride() override;					//譖ｴ譁ｰ
	void SubmitDrawsOverride(Renderer& renderer) override;		//謠冗判隕∵ｱよ署蜃ｺ
	void ResolveCollisionsOverride() override;		//陦晉ｪ∬ｧ｣豎ｺ
	void FinalizeOverride() override;					//邨ゆｺ・

	void SetPlayerData(std::vector<Player*>& players);				//繝励Ξ繧､繝､繝ｼ諠・ｱ縺ｮ險ｭ螳・
	void SetGameUIManager(GameUIManager* pUIManager);				//UI繝槭ロ繝ｼ繧ｸ繝｣繝ｼ縺ｮ險ｭ螳・
	void SetCollisionManager(CollisionManager* pCollisionManager);	//陦晉ｪ√・繝阪・繧ｸ繝｣繝ｼ縺ｮ險ｭ螳・
	void SetBB(int teamID, bool activate);						//BB縺ｮ逋ｺ蜍輔・辟｡蜉ｹ蛹冶ｨｭ螳・
	void OnItemPickup(int teamID);								//繧｢繧､繝・Β蜿門ｾ励さ繝ｼ繝ｫ繝舌ャ繧ｯ

private:
	BB* m_BB[BB_NUM] = { nullptr }; //BB驟榊・
	BilibiliArea *m_BBAreas[BB_AREA_NUM] = { nullptr };
	std::vector<WorldRenderInfo> m_LineBBInfo;			//BB繝ｩ繧､繝ｳ謠冗判諠・ｱ
	std::vector<WorldRenderInfo> m_ElectricityBBInfo;	//BB髮ｻ豌玲緒逕ｻ諠・ｱ
	std::vector<WorldRenderInfo> m_BBAreaInfo;	//BB繧ｨ繝ｪ繧｢謠冗判諠・ｱ

	float m_BBTimer[BB_NUM] = { 0.0f }; //BB繧ｿ繧､繝槭・
	FrameTimer m_frameTimer[BB_NUM];
	FrameTimer bbAreaStartEventTimer;

	std::vector<int> m_activationCalledBBIndex; //BB発動コマンド(インデックス指定)

	GameUIManager* m_pUIManager = nullptr; //UI繝槭ロ繝ｼ繧ｸ繝｣繝ｼ縺ｸ縺ｮ繝昴う繝ｳ繧ｿ
	CollisionManager* m_pCollisionManager = nullptr; //陦晉ｪ√・繝阪・繧ｸ繝｣繝ｼ縺ｸ縺ｮ繝昴う繝ｳ繧ｿ
private:
	void PrepareRenderInfo(	//BB謠冗判諠・ｱ逕滓・
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
};