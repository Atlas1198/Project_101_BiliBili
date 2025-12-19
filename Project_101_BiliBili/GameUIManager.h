#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "TextureManager.h"
#include "UIManagerBase.h"
#include "HPBarUI.h"
#include "BulletCountUI.h"
#include "IconUI.h"

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
	HPBarUI* m_pHPBarUI1 = nullptr;	//HPバーUIポインタ
	HPBarUI* m_pHPBarUI2 = nullptr;	//HPバーUIポインタ

	BulletCountUI* m_pBulletCountUI1 = nullptr; //弾数UIポインタ
	BulletCountUI* m_pBulletCountUI2 = nullptr; //弾数UIポインタ

	IconUI* m_pIconUI1 = nullptr; //アイコンUIポインタ
	IconUI* m_pIconUI2 = nullptr; //アイコンUIポインタ

private:
	void OnHPChanged(int teamID, float newHP); // HP変更時の処理
	void OnBulletCountChanged(int teamID, int newCount); // 弾数変更時の処理

private:
	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};