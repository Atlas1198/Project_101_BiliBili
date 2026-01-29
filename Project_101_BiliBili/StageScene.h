#pragma once
#include "SceneBase.h"
#include "StageSelector.h"
#include "StageUIManager.h"

//前方宣言
class InputManager;
class TextureManager;
class MeshManager;

//ステージシーンクラス
class StageScene : public SceneBase
{
public:	//公開関数
	StageScene(float window_width, float window_height);	//コンストラクタ
	~StageScene();											//デストラクタ

private:
	//メンバ変数
	StageSelector* m_pStageSelector = nullptr;	//ステージ選択クラス
	StageUIManager* m_pUIManager = nullptr;		//UI管理クラス

private:
	//メイン処理関数
	void InitializeOverride(				//初期化
		InputManager* pInputManager,		//入力管理クラスのポインタ
		TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
		MeshManager& pMeshManager			//メッシュ管理クラスの参照
	) override;
	void UpdateOverride() override;						//更新
	void ResolveCollisions() override {};				//衝突後処理
	void DrawOverride(Renderer& pRenderer) override;	//描画
	void FinalizeOverride() override;					//終了

	void ChangeStageUI(STAGE_TYPE stageType); //ステージUI変更関数
};