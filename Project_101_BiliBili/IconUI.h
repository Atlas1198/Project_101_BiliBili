#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"

//アイコンUIクラス
class IconUI : public UIBase
{
public:
	IconUI(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 scale,
		DirectX::XMFLOAT3 rotation,
		UINT order,
		const wchar_t* texturePath
	);
	~IconUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

	void SetTeamCharacter(int p1, int p2);

private:
	UIImage* m_pIconImage = nullptr;				//アイコン画像UIポインタ
	UIImage* m_pCharacterImages[4] = { nullptr };	//キャラクター画像UIポインタ配列

	const wchar_t* m_texturePath{ nullptr }; // テクスチャパス
private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};
