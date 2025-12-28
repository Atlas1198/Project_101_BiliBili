#pragma once
#include "UIBase.h"
#include "UIImage.h"

//リザルトUIクラス
class ResultUI : public UIBase
{
	public:
	//コンストラクタ
	ResultUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order);
	//デストラクタ
	~ResultUI() override = default;
	//初期化
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	//更新
	void UpdateOverride() override;
	//終了
	void FinalizeOverride() override;
	void ShowResult();	//リザルト表示関数(引数:勝利かどうか)

private:
	UIImage* m_pBackgroundImage = nullptr;	//背景画像UIポインタ
	int m_mainTimer = 0;				//メインタイマー

private:
	//オブジェクトの描画情報生成
	void PrepareRenderInfoOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
};