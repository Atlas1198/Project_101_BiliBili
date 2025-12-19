#pragma once
#include "SceneBase.h"
#include "ObjectBase.h"
#include "ObjectManagerBase.h"
#include "TitleUIManager.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

// タイトルシーンクラス
class TitleScene : public SceneBase
{
public:	//公開関数
	TitleScene(float window_width, float window_height);	//コンストラクタ
	~TitleScene();											//デストラクタ

	//メイン処理関数
	void InitializeOverride(									//初期化
		InputManager* pInputManager,		//入力管理クラスのポインタ
		TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
		MeshManager& pMeshManager			//メッシュ管理クラスの参照
	) override;
	void UpdateOverride() override;						//更新
	void ResolveCollisions() override {};				//衝突後処理
	void DrawOverride(Renderer& pRenderer) override;	//描画
	void FinalizeOverride() override;					//終了

	//ゲッター
	CameraInfo* GetCameraInfo() const;			//カメラ情報取得

private:

	TitleUIManager* m_pTitleUIManager = nullptr;
};