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

class BBManager : public ObjectManagerBase
{
public:
	static constexpr int BB_NUM = 2;
	static constexpr int BB_AREA_NUM = 4;
	const wchar_t* lineBBTexPath = L"asset/texture/line.png";
	const wchar_t* electricityBBTexPath = L"asset/texture/effect/bilibili.png";

	const wchar_t* electricityBBRedTexPath = L"asset/texture/effect/line_R_EF.png";
	const wchar_t* electricityBBBlueTexPath = L"asset/texture/effect/line_B_EF.png";

	const wchar_t* areaBBRedTexPath = L"asset/texture/effect/circle_R_EF.png";
	const wchar_t* areaBBBlueTexPath = L"asset/texture/effect/circle_B_EF.png";

	inline static float BB_DURATION = 15.0f;
	//static constexpr float BB_ENHANCE_TIME = 120.0f;
	static constexpr float BB_ENHANCE_TIME = 15.0f;

public:
	BBManager();		
	~BBManager();	
	
	void InitializeOverride(	
		TextureManager& textureManager,
		MeshManager& meshManager,
		CollisionManager& collisionManager
	) override;
	void UpdateOverride() override;					
	void SubmitDrawsOverride(Renderer& renderer) override;		
	void ResolveCollisionsOverride() override;		
	void FinalizeOverride() override;					

	void SetPlayerData(std::vector<Player*>& players);				
	void SetGameUIManager(GameUIManager* pUIManager);				
	void SetCollisionManager(CollisionManager* pCollisionManager);	
	void SetBB(int teamID, bool activate);						
	void OnItemPickup(int teamID);								

private:
	BB* m_BB[BB_NUM] = { nullptr }; 
	BilibiliArea *m_BBAreas[BB_AREA_NUM] = { nullptr };
	std::vector<WorldRenderInfo> m_LineBBInfo;			
	std::vector<WorldRenderInfo> m_ElectricityBBRedInfo;
	std::vector<WorldRenderInfo> m_ElectricityBBBlueInfo;
	std::vector<WorldRenderInfo> m_BBAreaRedInfo;
	std::vector<WorldRenderInfo> m_BBAreaBlueInfo;

	float m_BBTimer[BB_NUM] = { 0.0f }; 
	FrameTimer m_frameTimer[BB_NUM];
	FrameTimer bbAreaStartEventTimer;

	std::vector<int> m_activationCalledBBIndex; 

	GameUIManager* m_pUIManager = nullptr; 
	CollisionManager* m_pCollisionManager = nullptr; 

	bool m_isBBEnhanced = false;
	bool timerStarted = false;

private:
	void PrepareRenderInfo(	
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
};