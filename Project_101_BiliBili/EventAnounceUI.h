#pragma once
#include "UIBase.h"
#include "UIimage.h"
#include "EventType.h"

class EventAnounceUI : public UIBase
{
public:	//公開関数
	EventAnounceUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0,
		BLEND_MODE blendMode = BLEND_MODE::BLEND_TRANSPARENT
	);
	virtual ~EventAnounceUI() = default;	//デストラクタ

	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

	void ShowAnounce(EventType type); //アナウンス表示関数

private:
	UIImage* m_pBB = nullptr;
	UIImage* m_pSpeed = nullptr;
	UIImage* m_pRecover = nullptr;

	int m_timer = 0;
	int m_blinkCount = 0;

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override {
	};
};