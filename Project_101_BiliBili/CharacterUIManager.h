#pragma once
#include "UIManagerBase.h"
#include "UIImage.h"

//前方宣言
struct InputInfo;

class CharacterUIManager : public UIManagerBase
{
public:	//公開関数

public:
	CharacterUIManager(
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	)
		: UIManagerBase(screenWidth, screenHeight)
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
	UIImage* m_pBackgroundImage = nullptr;					//背景画像UIオブジェクト
	UIImage* m_pHeaderImage = nullptr;						//ヘッダー画像UIオブジェクト
	UIImage* m_pCharacterIconBase = nullptr;				//キャラクターアイコンベースUIオブジェクト
	UIImage* m_pPlayerIcons[4] = { nullptr };				//プレイヤーアイコンUIオブジェクト配列
	UIImage* m_pPlayerBackgroundsSelected[4] = { nullptr };	//プレイヤー背景UIオブジェクト配列(選択済み)
	UIImage* m_pPlayerBackgroundsNormal[4] = { nullptr };	//プレイヤー背景UIオブジェクト配列(未選択)
	UIImage* m_pGoToNextSceneIcon = nullptr;				//次のシーンへ進むアイコンUIオブジェクト
	UIImage* m_pOperation = nullptr;						//操作説明UIオブジェクト

	float m_passedFrameCount = 0.0f;			//経過フレーム数
	bool m_playersSelected[4] = { false };		//プレイヤー選択済みフラグ配列
	bool m_isGoToNextSceneIconVisible = false;	//次のシーンへ進むアイコン表示フラグ

	bool m_isCalledGoToNextScene = false;	//次のシーンへ進む処理が呼ばれたかどうか
	int m_goToNextSceneIconMoveCount = 0;	//次のシーンへ進むアイコン移動カウント

	float playerIconScaleOffsets[4] = { 1.0f, 1.0f, 1.0f, 1.0f }; //プレイヤーアイコンスケールオフセット配列

private:
	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了
	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};