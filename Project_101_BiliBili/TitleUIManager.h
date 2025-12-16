#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "TextureManager.h"
#include "UIManagerBase.h"
#include "TitleUI.h"

class TitleUIManager : public UIManagerBase
{
public:	//公開関数
	TitleUIManager() {};	//コンストラクタ
	~TitleUIManager();	//デストラクタ
	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了

private:
	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

private:
	TitleUI* m_pTitleUI = nullptr;
};