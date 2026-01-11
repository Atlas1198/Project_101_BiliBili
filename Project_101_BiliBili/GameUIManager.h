#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "TextureManager.h"
#include "UIManagerBase.h"
#include "TeamUI.h"
#include "BulletCountUI.h"
#include "CutInUI.h"
#include "CountUI.h"
#include "ResultUI.h"

//ゲームUI管理クラス
class GameUIManager : public UIManagerBase
{
public:	//公開関数
	GameUIManager(
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	) 
		: UIManagerBase(screenWidth, screenHeight)
	{};	//コンストラクタ
	~GameUIManager();	//デストラクタ

	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了

private:
	TeamUI* m_pTeamUI1 = nullptr; //チームUIポインタ
	TeamUI* m_pTeamUI2 = nullptr; //チームUIポインタ

	BulletCountUI* m_pBulletCountUI1 = nullptr; //弾数UIポインタ
	BulletCountUI* m_pBulletCountUI2 = nullptr; //弾数UIポインタ

	CutInUI* m_pCutInUI1 = nullptr; //カットインUIポインタ
	CutInUI* m_pCutInUI2 = nullptr; //カットインUIポインタ

	UIImage* m_pOperationGuideImage = nullptr; //操作ガイド画像UIポインタ

	CountUI* m_pCountUI = nullptr; //カウントUIポインタ

	ResultUI* m_pResultUI = nullptr; //リザルトUIポインタ
	
private:
	void OnHPChanged(int teamID, float newHP);				//HP変更時の処理
	void OnBulletCountChanged(int teamID, int newCount);	//弾数変更時の処理
	void CallCutIn(int teamID, int characterIndex);			//カットイン呼び出し関数
	void CountUIShow(int count);							//カウントUI表示関数
	void ShowStartUI();										//スタートUI表示関数
	void ShowFinishUI();									//フィニッシュUI表示関数
	void HideCountUI();										//カウントUI非表示関数
	void ShowResultUI(int winner, int character1ID, int character2ID);	//リザルトUI表示関数
};