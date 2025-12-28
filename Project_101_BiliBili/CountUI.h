#pragma once
#include "UIBase.h"
#include "UIImage.h"

//カウントUIクラス
class CountUI : public UIBase
{
public:
	//コンストラクタ
	CountUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order);
	//デストラクタ
	~CountUI() override = default;
	//初期化
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	//更新
	void UpdateOverride() override;
	//終了
	void FinalizeOverride() override;

	void ShowCountDown(int count);	//カウントダウン表示関数(引数:カウント数)
	void ShowStart();				//スタート表示関数
	void ShowFinish();				//フィニッシュ表示関数
	void HideAll();					//全て非表示関数

private:
	UIImage* m_pCountImage3 = nullptr;	//カウント3画像UIポインタ
	UIImage* m_pCountImage2 = nullptr;	//カウント2画像UIポインタ
	UIImage* m_pCountImage1 = nullptr;	//カウント1画像UIポインタ
	UIImage* m_pStartImage = nullptr;	//スタート画像UIポインタ
	UIImage* m_pFinishImage = nullptr;	//フィニッシュ画像UIポインタ

	int m_mainTimer = 0;				//メインタイマー

	DirectX::XMFLOAT3 m_countImageBaseSize[3]{ {0.0f, 0.0f, 0.0f} };	//カウント画像の基準サイズ配列

private:
	//オブジェクトの描画情報生成
	void PrepareRenderInfoOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;

	void UpdateNumberImage(UIImage& image, DirectX::XMFLOAT3 baseSize); //数字表示更新関数
	void UpdateStartImage(UIImage& image);								//スタート表示更新関数
	void UpdateFinishImage(UIImage& image);								//フィニッシュ表示更新関数
};