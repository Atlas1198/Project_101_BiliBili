#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "TextureManager.h"
#include "UIManagerBase.h"
#include "HPBarUI.h"

//ゲームUI管理クラス
class GameUIManager : public UIManagerBase
{
public:	//公開関数
	GameUIManager() {};	//コンストラクタ
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

private:
	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;
};