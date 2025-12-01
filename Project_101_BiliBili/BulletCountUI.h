#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"

class BulletCountUI : public UIBase
{
public:
	//テクスチャファイルパス定数(仮)
		static constexpr const wchar_t* BULLET_TEXTURE_PATH = L"asset/texture/UI/UI_Bullet.png"; // 弾数テクスチャパス
public:
	BulletCountUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0
	);
	~BulletCountUI() {};
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;
	void SetBulletCount(int count) { m_currentBulletCount = count; } // 弾数設定
private:
	UIImage* m_pBulletImage = nullptr;	//弾数画像UIポインタ
	int m_bulletCountMax = 6;			//最大弾数
	int m_currentBulletCount = 6;		//現在の弾数
	int m_frameCount = 0;				//フレームカウント

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};
