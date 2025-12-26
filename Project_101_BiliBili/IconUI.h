#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"

class IconUI : public UIBase
{
public:
	//テクスチャファイルパス定数(仮)
	static constexpr const wchar_t* BULLET_TEXTURE_PATH = L"asset/texture/game_scene/UI_INGAME_L_icon.png"; // 弾数テクスチャパス
public:
	IconUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0
	);
	~IconUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;
private:
	UIImage* m_pIconImage = nullptr;	//アイコン画像UIポインタ

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};
