#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "ObjectManagerBase.h"
#include "SharedStruct.h"
#include "Wall.h"
#include "WallPass.h"
#include "Ground.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

//フィールド管理クラス
class FieldManager : public ObjectManagerBase
{
	const wchar_t* wallTexPath = L"asset/texture/white.png";
	const wchar_t* wallPassTexPath = L"asset/texture/black.png";
	const wchar_t* groundTexPath = L"asset/texture/tile.PNG";
public:
	FieldManager();		//コンストラクタ
	~FieldManager();	//デストラクタ

private:	//非公開メンバ変数
	//メイン処理関数
	void InitializeOverride(	//初期化
		InputManager* pInputManager,		//入力マネージャーのポインタ
		TextureManager& textureManager,		//テクスチャ管理クラスの参照
		MeshManager& meshManager,			//メッシュ管理クラスの参照
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) override;
	void UpdateOverride() override;							//更新
	void SubmitDrawsOverride(Renderer& renderer) override;	//描画要求をシーンに提出
	void ResolveCollisionsOverride() override;				//衝突後処理
	void FinalizeOverride() override;						//終了

	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

private:	//非公開メンバ変数
	std::vector<Wall*> m_pWalls;						//壁オブジェクト配列
	std::vector<WallPass*> m_pWallPasses;						//壁オブジェクト配列
	std::vector<Ground*> m_pGrounds;					//地面オブジェクト配列
	std::vector<RenderData::RenderInfo> m_wallInfo;		//壁描画情報
	std::vector<RenderData::RenderInfo> m_wallPassInfo;		//壁描画情報
	std::vector<RenderData::RenderInfo> m_groundInfo;	//地面描画情報
};