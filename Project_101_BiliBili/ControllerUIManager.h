#pragma once
#include "UIManagerBase.h"
#include "SharedStruct.h"
#include "RenderData.h"
#include "UIimage.h"

//前方宣言
struct InputInfo;

//コントローラーUI管理クラス
class ControllerUIManager : public UIManagerBase
{
public:
	ControllerUIManager(
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	) 
		: UIManagerBase(screenWidth, screenHeight)
	{};
	~ControllerUIManager() {};
protected:
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

private:
	UIImage* m_pBackgroundImage = nullptr;
	UIImage* m_pHeaderImage = nullptr;
	UIImage* m_pControllerIconBase = nullptr;
	UIImage* m_pControllerIcons[4] = { nullptr, nullptr, nullptr, nullptr };
	UIImage* m_pGoToNextSceneIcon = nullptr;

	int m_passedFrameCount = 0;

private:
	void SetIconConnected(int index);
	void ConnectedIconReaction(int index, const InputInfo& inputInfo);
};

