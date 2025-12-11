#pragma once
#include <DirectXMath.h>
#include "SharedStruct.h"
#include <vector>

//前方宣言
class Renderer;
class TextureManager;
class MeshManager;
class MeshGPU;

//=======================================================================================================
//列挙体群
//=======================================================================================================
//ブレンドモード列挙体
enum BLEND_MODE
{
	BLEND_OPAQUE,		//不透明
	BLEND_MASKED,		//マスク
	BLEND_TRANSPARENT,	//透明
	BLEND_MAX			//最大数
};

//ビルボードタイプ
enum BILLBOARD_TYPE
{
	BILLBOARD_NONE,			//ビルボードなし
	BILLBOARD_SPHERICAL,	//全軸ビルボード
	BILLBOARD_CYLINDRICAL	//Y軸のみ
};

//=======================================================================================================
//描画情報構造体群
//=======================================================================================================
//共通描画記述構造体
struct CommonRenderDesc
{
	MeshGPU* pMeshGPU = nullptr;						//メッシュデータ
	uint32_t srvIndex = UINT32_MAX;						//SRVインデックス(テクスチャ)
	DirectX::XMFLOAT4 color = { 1,1,1,1 };				//表示色
	DirectX::XMFLOAT4 uvRect{ 0.0f, 0.0f, 1.0f, 1.0f };	//UV矩形
	BLEND_MODE blendMode = BLEND_MODE::BLEND_OPAQUE;	//ブレンドモード
};

//描画情報構造体
struct RenderInfo
{
	CommonRenderDesc common;				//共通描画記述構造体
	DirectX::XMMATRIX world = {};			//ワールド行列
	UINT startIndex = 0;					//開始インデックス
	INT  baseVertex = 0;					//基準インデックス
	DirectX::XMFLOAT3 position{};			//座標
	DirectX::XMFLOAT3 scale{};				//スケール
	BILLBOARD_TYPE billboardType 
		= BILLBOARD_TYPE::BILLBOARD_NONE;	//ビルボードタイプ
};

//エフェクト描画情報構造体
struct EffectRenderInfo
{
	CommonRenderDesc common;	//共通描画記述構造体
	DirectX::XMFLOAT3 center{};	//座標
	DirectX::XMFLOAT2 size{};	//スケール
};

//=======================================================================================================
//メッシュ・モデルデータ構造体
//=======================================================================================================
//メッシュデータ構造体
struct Mesh
{
	std::vector<Vertex> vertices;	//頂点データ配列
	size_t vertexCount = 0;			//頂点数
	std::vector<uint32_t> indices;	//インデックスデータ配列
	size_t indexCount = 0;			//インデックス数
	std::wstring texPath;			//テクスチャのファイル名
};

//モデルデータ構造体
struct Model
{
	std::vector<Mesh> meshes;	//メッシュデータ配列
};

//mesh type enumeration
enum MESH_TYPE
{
	IMPORT,		//imported model
	QUAD,		//quad plane
	CUBE,		//cube
	CIRCLE,		//circle plane
	SPHERE,		//sphere
	CAPSULE,	//capsule
	CYLINDER,	//cylinder
};

//=======================
//四角平面
//=======================
//四角平面の頂点データ
inline constexpr Vertex QuadVertices[4] =
{
	{{-0.5f,  0.5f, 0.f},{0,0,1},{0,0},{1,0,0},{1,1,1,1}},	//頂点0
	{{ 0.5f,  0.5f, 0.f},{0,0,1},{1,0},{1,0,0},{1,1,1,1}},	//頂点1
	{{ 0.5f, -0.5f, 0.f},{0,0,1},{1,1},{1,0,0},{1,1,1,1}},	//頂点2
	{{-0.5f, -0.5f, 0.f},{0,0,1},{0,1},{1,0,0},{1,1,1,1}},	//頂点3
};

//四角平面のインデックスデータ
inline constexpr uint32_t QuadIndices[6] =
{
	0,1,2,	//三角形1
	0,2,3	//三角形2
};

//四角平面のメッシュデータ作成関数
Model MakeQuadModel();

//=======================
//立方体
//=======================
//立方体の頂点データ(24頂点)
inline constexpr Vertex CubeVertices[24] =
{
	// +Z
	{{-0.5,  0.5,  0.5}, {0,0,1}, {0,0}, {1,0,0}, {1,1,1,1}},	//頂点0
	{{ 0.5,  0.5,  0.5}, {0,0,1}, {1,0}, {1,0,0}, {1,1,1,1}},	//頂点1
	{{ 0.5, -0.5,  0.5}, {0,0,1}, {1,1}, {1,0,0}, {1,1,1,1}},	//頂点2
	{{-0.5, -0.5,  0.5}, {0,0,1}, {0,1}, {1,0,0}, {1,1,1,1}},	//頂点3

	// -Z
	{{ 0.5,  0.5, -0.5}, {0,0,-1}, {0,0}, {-1,0,0}, {1,1,1,1}},	//頂点4
	{{-0.5,  0.5, -0.5}, {0,0,-1}, {1,0}, {-1,0,0}, {1,1,1,1}},	//頂点5
	{{-0.5, -0.5, -0.5}, {0,0,-1}, {1,1}, {-1,0,0}, {1,1,1,1}},	//頂点6
	{{ 0.5, -0.5, -0.5}, {0,0,-1}, {0,1}, {-1,0,0}, {1,1,1,1}},	//頂点7

	// +X
	{{ 0.5,  0.5,  0.5}, {1,0,0}, {0,0}, {0,0,-1}, {1,1,1,1}},	//頂点1
	{{ 0.5,  0.5, -0.5}, {1,0,0}, {1,0}, {0,0,-1}, {1,1,1,1}},	//頂点5
	{{ 0.5, -0.5, -0.5}, {1,0,0}, {1,1}, {0,0,-1}, {1,1,1,1}},	//頂点6
	{{ 0.5, -0.5,  0.5}, {1,0,0}, {0,1}, {0,0,-1}, {1,1,1,1}},	//頂点2

	// -X
	{{-0.5,  0.5, -0.5}, {-1,0,0}, {0,0}, {0,0,1}, {1,1,1,1}},	//頂点4
	{{-0.5,  0.5,  0.5}, {-1,0,0}, {1,0}, {0,0,1}, {1,1,1,1}},	//頂点0
	{{-0.5, -0.5,  0.5}, {-1,0,0}, {1,1}, {0,0,1}, {1,1,1,1}},	//頂点3
	{{-0.5, -0.5, -0.5}, {-1,0,0}, {0,1}, {0,0,1}, {1,1,1,1}},	//頂点7

	// +Y
	{{-0.5,  0.5, -0.5}, {0,1,0}, {0,0}, {1,0,0}, {1,1,1,1}},		//頂点4
	{{ 0.5,  0.5, -0.5}, {0,1,0}, {1,0}, {1,0,0}, {1,1,1,1}},		//頂点5
	{{ 0.5,  0.5,  0.5}, {0,1,0}, {1,1}, {1,0,0}, {1,1,1,1}},		//頂点1
	{{-0.5,  0.5,  0.5}, {0,1,0}, {0,1}, {1,0,0}, {1,1,1,1}},		//頂点0

	// -Y
	{{-0.5, -0.5,  0.5}, {0,-1,0}, {0,0}, {1,0,0}, {1,1,1,1}},	//頂点3
	{{ 0.5, -0.5,  0.5}, {0,-1,0}, {1,0}, {1,0,0}, {1,1,1,1}},	//頂点2
	{{ 0.5, -0.5, -0.5}, {0,-1,0}, {1,1}, {1,0,0}, {1,1,1,1}},	//頂点6
	{{-0.5, -0.5, -0.5}, {0,-1,0}, {0,1}, {1,0,0}, {1,1,1,1}},	//頂点7
};

//立方体のインデックスデータ
inline constexpr uint32_t CubeIndices[36] =
{
	// +Z
	0,1,2,  0,2,3,			//三角形1、2
	// -Z
	4,6,5,  4,7,6,			//三角形3、4
	// +X
	8,9,10,  8,10,11,		//三角形5、6
	// -X
	12,13,14,  12,14,15,	//三角形7、8
	// +Y
	16,17,18,  16,18,19,	//三角形9、10
};

//立方体のメッシュデータ作成関数
Model MakeCubeModel();

//=======================
//円形平面
//=======================
//円形平面のメッシュデータ作成関数
Model MakeCircleModel(int slice = 32);

//=======================
//球体
//=======================
//球体のメッシュデータ作成関数
Model MakeSphereModel(int slice = 32, int stacks = 16);

//=======================
//カプセル
//=======================
//カプセルのメッシュデータ作成関数
Model MakeCapsuleModel(int slice = 32, int stacks = 16);

//カプセルのビジュアル記述構造体
struct CapsuleVisualDesc
{
	float baseRadius = 0.5f;		//底面半径
	float basehalfHeight = 0.5f;	//半分の高さ
};

//カプセルの描画情報追加関数
void AppendCapsuleRenderInfos(
	const CapsuleVisualDesc& desc,				//カプセル描画情報記述子
	const DirectX::XMFLOAT3& position,			//位置
	const DirectX::XMFLOAT3& scale,				//スケール
	const DirectX::XMFLOAT3& rotEuler,			//回転Euler角
	const DirectX::XMFLOAT4& color,				//色
	std::vector<RenderInfo>& infos,	//入力元描画情報配列
	std::vector<RenderInfo>& out	//出力先描画情報配列
);

//=======================
//円柱
//=======================
//円柱のメッシュデータ作成関数
Model MakeCylinderModel(int slice = 32, int stacks = 16);

//メッシュデータ取得関数
inline Model GetModel(MESH_TYPE type)
{
	//メッシュタイプに応じたメッシュデータを返す
	switch (type)
	{
	case QUAD: return MakeQuadModel();			//四角平面
	case CUBE: return MakeCubeModel();			//立方体
	case SPHERE: return MakeSphereModel();		//球体
	case CIRCLE: return MakeCircleModel();		//円形平面
	case CAPSULE: return MakeCapsuleModel();	//カプセル
	case CYLINDER: return MakeCylinderModel();	//円柱
	default:   return {};						//その他
	}
}

//=======================================================================================================
//描画情報作成関数群
//=======================================================================================================
//モデルデータ又はテクスチャファイルから描画情報を作成する関数
void CreateRenderInfo(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,			//描画情報構造体配列へのポインタ
	MESH_TYPE mType,				//メッシュタイプ
	BLEND_MODE mode,						//ブレンドモード
	const wchar_t* path,					//モデルデータ又はテクスチャファイルのパス
	BILLBOARD_TYPE bType = BILLBOARD_NONE,	//ビルボードタイプ
	bool inverseU = false,					//Uを反転するかどうか(モデルデータの場合のみ有効)
	bool inverseV = false					//Vを反転するかどうか(モデルデータの場合のみ有効)
);

//FBXファイルから描画情報を作成する関数
void CreateRenderInfoFromFBX(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,			//描画情報構造体配列へのポインタ
	BLEND_MODE mode,						//ブレンドモード
	const wchar_t* path,					//モデルファイルのパス
	BILLBOARD_TYPE bType = BILLBOARD_NONE,	//ビルボードタイプ
	bool inverseU = false,					//Uを反転するかどうか
	bool inverseV = false					//Vを反転するかどうか
);

//デフォルトのメッシュデータから描画情報を作成する関数
void CreateRenderInfoFromDefaultMesh(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,			//描画情報構造体配列へのポインタ
	MESH_TYPE type,				//メッシュタイプ
	BLEND_MODE mode,						//ブレンドモード
	const wchar_t* path,					//テクスチャのファイル名
	BILLBOARD_TYPE bType = BILLBOARD_NONE	//ビルボードタイプ
);

void CreateEffectRenderInfo(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	std::vector<EffectRenderInfo>* pInfo,	//エフェクト描画情報構造体配列へのポインタ
	MESH_TYPE type,				//メッシュタイプ
	BLEND_MODE mode,						//ブレンドモード
	const wchar_t* path						//テクスチャのファイル名
);

//メッシュデータから描画情報を構築する関数
CommonRenderDesc CreateRenderInfoFromMeshData(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	Mesh& mesh,					//メッシュデータ構造体への参照
	BLEND_MODE mode,						//ブレンドモード
	BILLBOARD_TYPE bType = BILLBOARD_NONE	//ビルボードタイプ
);