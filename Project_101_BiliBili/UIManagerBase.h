#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <vector>
#include <memory>
#include <utility>
#include "UIBase.h"
#include "SharedStruct.h"
#include "RenderData.h"

// 前方宣言
class Renderer;
class TextureManager;
class MeshManager;

//UI管理クラス
class UIManagerBase
{
public:	//公開関数
	UIManagerBase() {};	//コンストラクタ
	~UIManagerBase();	//デストラクタ
	//メイン処理関数
	void Initialize(										//初期化
		TextureManager& textureManager,	//テクスチャ管理クラス
		MeshManager& meshManager		//メッシュ管理クラス
	);
	void Update();											//更新
	void SubmitDraws(Renderer& renderer);					//描画要求をシーンに提出
	void Finalize();										//終了

protected:
	virtual void InitializeOverride(							//初期化(派生クラスでオーバーライド)
		TextureManager& textureManager,
		MeshManager& meshManager
	) = 0;
	virtual void UpdateOverride() = 0;							//更新(派生クラスでオーバーライド)
	virtual void FinalizeOverride() = 0;						//終了(派生クラスでオーバーライド)

	virtual void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) = 0;

	void SubmitRenderInfo(	//描画情報をシーンに提出
		Renderer& renderer,							//シーンの参照
		std::vector<WorldRenderInfo>& info	//描画情報構造体
	);

protected:
	std::vector<std::unique_ptr<UIBase>> m_roots;		//ルートUIオブジェクト配列
};