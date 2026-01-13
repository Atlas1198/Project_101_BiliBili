#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"


class TitleUI : public UIBase
{
public:
	//テクスチャファイルパス定数(仮)
	static constexpr const wchar_t* FRAME_TEXTURE_PATH = L"asset/texture/title_scene/UI_TITLE_Frame.png";
	static constexpr const wchar_t* BUTTON_TEXTURE_PATH = L"asset/texture/title_scene/UI_TITLE_Button.png";
	static constexpr const wchar_t* BACK_TEXTURE_PATH = L"asset/texture/title_scene/UI_TITLE_Back.png";
public:
	TitleUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0
	);
	~TitleUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;
private:
	UIImage* m_pFrameImage = nullptr;
	UIImage* m_pButtonImage = nullptr;
	UIImage* m_pBackImage = nullptr;

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

	//void UpdateStartPromptImage();

};