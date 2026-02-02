#pragma once
#include "SceneBase.h"
#include "ControllerConnecter.h"
#include "ControllerUIManager.h"

//前方宣言
class InputManager;
class TextureManager;
class MeshManager;

//コントローラーシーンクラス
class ControllerScene : public SceneBase
{
	public:
	ControllerScene(float window_width, float window_height);	//コンストラクタ
	~ControllerScene();											//デストラクタ
	void ResolveCollisions() override {};						//衝突後処理

private:
	ControllerConnecter* m_pControllerConnecter = nullptr;	//コントローラー接続クラス
	ControllerUIManager* m_pUIManager = nullptr;				//コントローラーUI管理クラス

private:
	void InitializeOverride(					//シーン固有の初期化
		TextureManager& pTextureManager,
		MeshManager& pMeshManager
	) override;
	void UpdateOverride() override;					//シーン固有の更新
	void DrawOverride(Renderer& pRenderer) override;	//シーン固有の描画
	void FinalizeOverride() override;					//シーン固有の終了
};