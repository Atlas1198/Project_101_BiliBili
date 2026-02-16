#pragma once
#include "UIBase.h"
#include "UIimage.h"

class BBSceneEffectUI : public UIBase
{
public:
	BBSceneEffectUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0,
		const std::wstring& texturePath = L"",
		PSOKey psoKey = PSO_KEY_TRANSPARENT
	);
	~BBSceneEffectUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

protected:
	void PrepareRenderInfoOverride(	//描画情報生成
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;

private:
	UIImage* m_pTopImage = nullptr;		//トップイメージ
	UIImage* m_pBottomImage = nullptr;	//ボトムイメージ

	float m_uOffset = 0.0f;
	float m_time = 0.0f;
};
