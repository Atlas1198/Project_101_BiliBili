#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"


// HPバーUIクラス
class HPBarUI : public UIBase
{
public:
	//テクスチャファイルパス定数(仮)
	static constexpr const wchar_t* FRAME_TEXTURE_PATH = L"asset/texture/UI/UI_HP_Frame.png"; // フレームテクスチャパス
	static constexpr const wchar_t* GAGE_TEXTURE_PATH = L"asset/texture/UI/UI_HP_Gage.png"; // バーテクスチャパス
	static constexpr const wchar_t* BASE_TEXTURE_PATH = L"asset/texture/UI/UI_HP_Base.png"; // 背景テクスチャパス
public:
	HPBarUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0
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