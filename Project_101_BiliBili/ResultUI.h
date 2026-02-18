#pragma once
#include "UIBase.h"
#include "UIImage.h"
#include "Random.h"
#include "CharacterData.h"

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

	bool isGoToTitleShown() const { return m_isGoToTitleShown; }					//タイトルへ戻る表示フラグゲッター
	void SetGoToTitleTimer(float duration) { m_goToTitlePressAmount = duration; }	//タイトルへ戻る表示タイマースタート関数
	void DropThankyouForPlaying() {													// "Thank you for playing"表示関数
		m_pThankyouForPlaying->SetActive(true); 
		m_ThankYouForPlayingFallTimer = 0; //落下タイマーリセット
	}
	void ShakeGoToTitleButton(); //タイトルへ戻るボタンを揺らす関数

private:
	UIImage* m_pBackgroundImage[TEAM_NUM] = { nullptr };					//背景画像UIポインタ
	UIImage* m_pTeamTextImage[TEAM_NUM] = { nullptr };						//チームテキスト画像UIポインタ
	UIImage* m_pHeaderImage = nullptr;										//ヘッダー画像UIポインタ
	UIImage* m_pFooterImage[TEAM_NUM] = { nullptr };						//フッター画像UIポインタ
	UIImage* m_pGoToTitleImage = nullptr;									//タイトルへ戻る画像UIポインタ
	UIImage* m_pGoToTitleImageWrap = nullptr;								//タイトルへ戻る画像UIポインタ
	UIImage* m_pCharacterImage[TEAM_NUM][MAX_CHARACTER_NUM] = { nullptr };	//キャラクター画像UIポインタ配列
	UIImage* m_pItemImage = nullptr;										//アイテム画像UIポインタ
	UIImage* m_pConfettiImage[100] = { nullptr };							//紙吹雪画像UIポインタ
	UIImage* m_pThankyouForPlaying = nullptr;

	Random* m_pRandom = nullptr;

	bool m_isResultShown = false;			//リザルト表示フラグ
	bool m_isGoToTitleShown = false;		//タイトルへ戻る表示フラグ
	bool m_confettiActive = false;			//紙吹雪表示フラグ
	bool m_shakeGoToTitleButton = false;	//タイトルへ戻るボタンを揺らすフラグ
	int m_mainTimer = 0;					//メインタイマー
	int m_confettiTimer = 0;				//紙吹雪タイマー
	int m_ThankYouForPlayingFallTimer = 0;	//Thank you for playing落下タイマー
	int m_winner = -1;						//勝利チームID
	int m_character1ID = -1;				//キャラクター1ID
	int m_character2ID = -1;				//キャラクター2ID
	float m_goToTitlePressAmount = 0;		//タイトルへ戻るボタンが押されている時間(0.0f～1.0f)
private:
	//オブジェクトの描画情報生成
	void PrepareRenderInfoOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;

	void UpdateConfetti(); //紙吹雪更新関数
};