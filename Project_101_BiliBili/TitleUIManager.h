#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "TextureManager.h"
#include "UIManagerBase.h"
#include "UIImage.h"

class TitleUIManager : public UIManagerBase
{
public:	//公開関数
	TitleUIManager(CameraInfo* cameraInfo, float screenWidth, float screenHeight)
		: UIManagerBase(cameraInfo, screenWidth, screenHeight) {}
	~TitleUIManager();	//デストラクタ

	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了

private:
	UIImage* m_pFrameImage = nullptr;
	UIImage* m_pButtonImage = nullptr;
	UIImage* m_pBackImage = nullptr;

	int m_timer = 0;
};