#pragma once
#include "UIBase.h"
#include "UIimage.h"

//ロードUIクラス
class LoadUI : public UIBase
{
public:	//公開関数
	LoadUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order);
	~LoadUI() override = default;
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

protected:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,
		MeshManager& meshManager
	) override {
	};

private:
	UIImage* m_pBackgroundImage = nullptr;	//背景画像UIポインタ
};