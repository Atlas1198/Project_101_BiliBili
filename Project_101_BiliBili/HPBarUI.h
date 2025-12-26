#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"


// HPバーUIクラス
class HPBarUI : public UIBase
{
public:
	HPBarUI(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 scale,
		DirectX::XMFLOAT3 rotation,
		UINT order,
		const wchar_t* frameTexturePath,
		const wchar_t* gageTexturePath,
		const wchar_t* baseTexturePath
	);
	~HPBarUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;
private:
	UIImage* m_pFrameImage = nullptr;	//フレーム画像UIポインタ
	UIImage* m_pGageImage = nullptr;	//ゲージ画像UIポインタ
	UIImage* m_pBaseImage = nullptr;	//背景画像UIポインタ

	const wchar_t* m_frameTexturePath = nullptr;	//テクスチャパスバッファ
	const wchar_t* m_gageTexturePath = nullptr;	//テクスチャパスバッファ
	const wchar_t* m_baseTexturePath = nullptr;	//テクスチャパスバッファ

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

	void UpdateGageImage();	//ゲージ画像更新関数


public:
	void SetHealth(float health);

private:
	float m_hpRate = 1.0f;		//HP割合(0.0~1.0)
	float m_hpMax = 1.0f;	//最大HP
};