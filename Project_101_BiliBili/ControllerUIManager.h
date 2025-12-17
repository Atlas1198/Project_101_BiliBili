#pragma once
#include "UIManagerBase.h"
#include "SharedStruct.h"
#include "RenderData.h"

//コントローラーUI管理クラス
class ControllerUIManager : public UIManagerBase
{
public:
	ControllerUIManager() {};
	~ControllerUIManager() {};
protected:
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;
	void PrepareRenderInfo(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;

private:
	void SetIconConnected(int index);
	void ConnectedIconReaction(int index, const InputInfo& inputInfo);
};

