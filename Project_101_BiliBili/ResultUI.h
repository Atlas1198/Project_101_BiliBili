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

	void ShowResult(int winner, int character1ID, int character2ID);	//リザルト表示関数

private:
	UIImage* m_pBackgroundImage = nullptr;			//背景画像UIポインタ
	UIImage* m_pHeaderImage = nullptr;				//ヘッダー画像UIポインタ
	UIImage* m_pFooterImage = nullptr;				//フッター画像UIポインタ
	UIImage* m_pWinnerImage = nullptr;				//勝者画像UIポインタ
	UIImage* m_pBlueImage = nullptr;				//青チーム画像UIポインタ
	UIImage* m_pRedImage = nullptr;					//赤チーム画像UIポインタ
	UIImage* m_pGoToTitleImage = nullptr;			//タイトルへ戻る画像UIポインタ
	UIImage* m_pCharacterImage[4] = { nullptr };	//キャラクター画像UIポインタ配列

	int m_mainTimer = 0;	//メインタイマー

private:
	//オブジェクトの描画情報生成
	void PrepareRenderInfoOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
};