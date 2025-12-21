#pragma once
#include "UIManagerBase.h"

//前方宣言
struct InputInfo;

class CharacterUIManager : public UIManagerBase
{
public:
	CharacterUIManager(
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	)
		: UIManagerBase(screenWidth, screenHeight)
	{};	//コンストラクタ
	~CharacterUIManager();	//デストラクタ

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