#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include "ObjectManagerBase.h"
#include "SharedStruct.h"
#include "Wall.h"
#include "WallPass.h"
#include "WallCurve.h"
#include "Ground.h"
#include "Spring.h"
#include "NoCollisionModel.h"

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
	const wchar_t* wallCurveFbxPath = L"asset/fbx/CurveWall2.fbx";
	const wchar_t* groundTexPath = L"asset/texture/tile.PNG";
	const wchar_t* springTexPath = L"asset/fbx/screw/ST_screw.fbx";
public:
	FieldManager();		//コンストラクタ
	~FieldManager();	//デストラクタ

private:	//非公開メンバ変数
	//メイン処理関数
	void InitializeOverride(	//初期化
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
	std::vector<Wall*> m_pWalls;					//壁オブジェクト配列
	std::vector<WallPass*> m_pWallPasses;			//貫通壁オブジェクト配列
	std::vector<WallCurve*> m_pWallCurves;			//カーブ壁オブジェクト配列
	std::vector<Ground*> m_pGrounds;				//地面オブジェクト配列
	std::vector<Spring*> m_pSprings;				//バネオブジェクト配列
	std::vector<NoCollisionModel*> m_pDrivers;		//ドライバーオブジェクト配列
	std::vector<NoCollisionModel*> m_pSprays_R;		//スプレーオブジェクト配列
	std::vector<NoCollisionModel*> m_pSprays_B;		//スプレーオブジェクト配列
	std::vector<NoCollisionModel*> m_pChips;		//チップオブジェクト配列
	std::vector<NoCollisionModel*> m_pBatterys;		//バッテリーオブジェクト配列
	std::vector<NoCollisionModel*> m_pSpanners;		//スパナオブジェクト配列

	std::vector<WorldRenderInfo> m_wallInfo;		//壁描画情報
	std::vector<WorldRenderInfo> m_wallPassInfo;	//貫通壁描画情報
	std::vector<WorldRenderInfo> m_wallCurveInfo;	//カーブ壁描画情報
	std::vector<WorldRenderInfo> m_groundInfo;		//地面描画情報
	std::vector<WorldRenderInfo> m_springInfo;		//バネ描画情報
	std::vector<WorldRenderInfo> m_driverInfo;		//ドライバー描画情報
	std::vector<WorldRenderInfo> m_spray_rInfo;		//スプレー描画情報
	std::vector<WorldRenderInfo> m_spray_bInfo;		//スプレー描画情報
	std::vector<WorldRenderInfo> m_chipInfo;		//チップ描画情報
	std::vector<WorldRenderInfo> m_batteryInfo;		//バッテリー描画情報
	std::vector<WorldRenderInfo> m_spannerInfo;		//スパナ描画情報
};