#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "UIBase.h"
#include "UIImage.h"

class BulletCountUI : public UIBase
{
public:
	BulletCountUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0,
		std::wstring bulletPass = L"",
		std::wstring crossPass = L""
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
	UIImage* m_pCrossImage = nullptr;
	int m_bulletCountMax = 6;			//最大弾数
	int m_currentBulletCount = 6;		//現在の弾数
	int m_frameCount = 0;				//フレームカウント

	std::wstring m_bulletPath = {};
	std::wstring m_crossPath = {};

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};
