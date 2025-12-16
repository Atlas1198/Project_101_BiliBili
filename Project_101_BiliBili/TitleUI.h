#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"


class TitleUI : public UIBase
{
public:
	//テクスチャファイルパス定数(仮)
	static constexpr const wchar_t* TITLE_LOG_TEXTURE_PATH = L"asset/texture/UI/Title_Log.png";
	static constexpr const wchar_t* START_PROMPT_TEXTURE_PATH = L"asset/texture/UI/Start_Prompt.png";
	static constexpr const wchar_t* BASE_TEXTURE_PATH = L"asset/texture/UI/white.png";
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
	UIImage* m_pLogImage = nullptr;
	UIImage* m_pPromptImage = nullptr;
	UIImage* m_pBaseImage = nullptr;

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

	//void UpdateStartPromptImage();

};