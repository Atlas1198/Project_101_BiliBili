#pragma once
#include "UIManagerBase.h"
#include "UIImage.h"
#include "CharacterData.h"
#include "LoadUI.h"

//前方宣言
struct InputInfo;

class CharacterUIManager : public UIManagerBase
{
public:	//公開関数

public:
	CharacterUIManager(
		CameraInfo* cameraInfo,
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	)
		: UIManagerBase(cameraInfo, screenWidth, screenHeight)
	{};	//コンストラクタ
	~CharacterUIManager();	//デストラクタ

	void MoveCharacterIcon(	//キャラクターアイコン移動
		int playerIndex,	//プレイヤーインデックス
		int characterIndex	//キャラクターインデックス
	);
	void ActivateSelectedPlayerBackground(int playerIndex, int characterIndex);		//選択済みプレイヤー背景を有効化
	void DeactivateSelectedPlayerBackground(int playerIndex, int characterIndex);	//選択済みプレイヤー背景を無効化
	void GoToNextScene();															//次のシーンへ進む

private:
	UIImage* m_pBackgroundImage = nullptr;									//背景画像UIオブジェクト
	UIImage* m_pHeaderImage = nullptr;										//ヘッダー画像UIオブジェクト
	UIImage* m_pCharacterIconBase = nullptr;								//キャラクターアイコンベースUIオブジェクト
	UIImage* m_pCharacterName = nullptr;									//キャラクター名UIオブジェクト
	UIImage* m_pPlayerIcons[MAX_CHARACTER_NUM] = { nullptr };				//プレイヤーアイコンUIオブジェクト配列
	UIImage* m_pPlayerBlurs[MAX_CHARACTER_NUM] = { nullptr };				//プレイヤーブラーUIオブジェクト配列
	UIImage* m_pPlayerBackgroundsSelected[MAX_CHARACTER_NUM] = { nullptr };	//プレイヤー背景UIオブジェクト配列(選択済み)
	UIImage* m_pPlayerBackgroundsNormal[MAX_CHARACTER_NUM] = { nullptr };	//プレイヤー背景UIオブジェクト配列(未選択)
	UIImage* m_pGoToNextSceneBack = nullptr;								//次のシーンへ進むアイコンUIオブジェクト(背景)
	UIImage* m_pGoToNextSceneText = nullptr;								//次のシーンへ進むアイコンUIオブジェクト(テキスト)
	UIImage* m_pOperation = nullptr;										//操作説明UIオブジェクト
	LoadUI* m_pLoadUI = nullptr;											//ロードUIオブジェクト

	float m_passedFrameCount = 0.0f;							//経過フレーム数
	bool m_playersSelected[MAX_CHARACTER_NUM] = { false };		//プレイヤー選択済みフラグ配列
	bool m_isGoToNextSceneIconVisible = false;					//次のシーンへ進むアイコン表示フラグ

	bool m_isCalledGoToNextScene = false;	//次のシーンへ進む処理が呼ばれたかどうか
	int m_goToNextSceneIconMoveCount = 0;	//次のシーンへ進むアイコン移動カウント

	float playerIconScaleOffsets[4] = { 1.0f, 1.0f, 1.0f, 1.0f };	//プレイヤーアイコンスケールオフセット配列

private:
	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了
};