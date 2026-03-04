#pragma once
#include "UIimage.h"
#include "UIBase.h"

//BBゲージUIクラス
class BBGageUI : public UIBase
{
public:
	BBGageUI(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 scale,
		DirectX::XMFLOAT3 rotation,
		UINT order,
		DIRECTION offsetDirection,
		const wchar_t* gageTexturePath
	);
	~BBGageUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;
	void PrepareRenderInfoOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override {
	};
	void SetGage(float ratio); //ゲージの割合設定関数

private:
	UIImage* m_pGageImage = nullptr;						//ゲージ画像UIポインタ
	DIRECTION m_gageDecrementDirection = DIRECTION::NONE;	//ゲージ減少方向
	float m_pTimeRate = 0.0f;								//時間経過による割合
};