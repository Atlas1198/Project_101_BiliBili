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
#include "EventAnounceUI.h"
#include "BBSceneEffectUI.h"

//ゲームUI管理クラス
class GameUIManager : public UIManagerBase
{
public:	//公開関数
	GameUIManager(
		CameraInfo* cameraInfo,
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	) 
		: UIManagerBase(cameraInfo, screenWidth, screenHeight)
	{};	//コンストラクタ
	~GameUIManager();	//デストラクタ

	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了
	void SetPlayerChasingUIPosition(						//弾数UI位置設定関数
		int teamID,									//チームID
		const DirectX::XMFLOAT3& position1,		//弾数UI1位置
		const DirectX::XMFLOAT3& position2		//弾数UI2位置
	);

	bool IsGoToTitleShown() const { return m_pResultUI->isGoToTitleShown(); } //タイトルへ戻る表示フラグゲッター

private:
	TeamUI* m_pTeamUI1 = nullptr; //チームUIポインタ
	TeamUI* m_pTeamUI2 = nullptr; //チームUIポインタ

	BulletCountUI* m_pBulletCountUI1[2] = { nullptr }; //弾数UIポインタ
	BulletCountUI* m_pBulletCountUI2[2] = { nullptr }; //弾数UIポインタ

	UIImage* m_pPlayerPointerImage[4] = { nullptr }; //プレイヤーポインター画像UIポインタ配列

	CutInUI* m_pCutInUI1 = nullptr; //カットインUIポインタ
	CutInUI* m_pCutInUI2 = nullptr; //カットインUIポインタ

	UIImage* m_pOperationGuideImage = nullptr; //操作ガイド画像UIポインタ

	CountUI* m_pCountUI = nullptr; //カウントUIポインタ

	EventAnounceUI* m_pEventAnounceUI = nullptr; //イベントアナウンスUIポインタ

	UIImage* m_pGoalAnounceImage = nullptr; //ゴールアナウンス画像UIポインタ

	ResultUI* m_pResultUI = nullptr; //リザルトUIポインタ

	BBSceneEffectUI* m_pBBEffect = nullptr;

	bool m_showGoalAounce = false;
	
private:
	void OnHPChanged(int teamID, float newHP);							//HP変更時の処理
	void OnBulletCountChanged(int teamID, int newCount);				//弾数変更時の処理
	void CallCutIn(int teamID, int characterIndex);						//カットイン呼び出し関数
	void ShowCountUI(int count);										//カウントUI表示関数
	void ShowStartUI();													//スタートUI表示関数
	void ShowFinishUI();												//フィニッシュUI表示関数
	void HideCountUI();													//カウントUI非表示関数
	void ShowResultUI(int winner, int character1ID, int character2ID);	//リザルトUI表示関数
	void SetBulletCountActive(int teamID, bool isActive);				//弾数UIアクティブ設定関数
	void InactivatePlayerPointerImages();								//プレイヤーポインター画像非アクティブ化関数
};