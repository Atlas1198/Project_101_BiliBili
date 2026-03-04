#pragma once
#include "UIBase.h"
#include "UIimage.h"

class TextureManager;
class MeshManager;

class IconUI : public UIBase
{
public:
	IconUI(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 scale,
		DirectX::XMFLOAT3 rotation,
		UINT order,
		const wchar_t* baseTexturePath,
		const wchar_t* bilibiliTexturePath
	);
	~IconUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

	void SetTeamCharacter(int p1, int p2);

	void TurnOnBiliBiliUI();
	void SetIsBBActivated(bool isActivated);

private:
	UIImage* m_pIconImage = nullptr;				//アイコン画像UIポインタ
	UIImage* m_pCharacterImages[4] = { nullptr };	//キャラクター画像UIポインタ配列
	UIImage* m_pBBImage = nullptr;

	bool m_is_turning_on_bb_ui = false;
	bool m_is_bb_activated = false;
	int m_bbUITimer = 0;

	const wchar_t* m_texturePath{ nullptr }; // テクスチャパス
private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};
