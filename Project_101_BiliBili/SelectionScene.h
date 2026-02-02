#pragma once
#include "SceneBase.h"

class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

class SelectionScene : public SceneBase
{
public:	//公開関数
	SelectionScene(float window_width, float window_height);	//コンストラクタ
	~SelectionScene();											//デストラクタ

	//メイン処理関数
	void InitializeOverride(									//初期化
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


};