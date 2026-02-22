#pragma once
#include <DirectXMath.h>
#include "AssimpNodeTransformAnim.h"
#include "SharedStruct.h"
#include <vector>

//前方宣言
class Renderer;
class TextureManager;
class MeshManager;
class MeshGPU;
struct NodeAnimationAsset;

//=======================================================================================================
//列挙体群
//=======================================================================================================

//ビルボードタイプ
enum BILLBOARD_TYPE
{
	BILLBOARD_NONE,			//ビルボードなし
	BILLBOARD_SPHERICAL,	//全軸ビルボード
	BILLBOARD_FIX_X,		//X軸のみ
	BILLBOARD_FIX_Y,		//Y軸のみ
	BILLBOARD_FIX_Z,		//Z軸のみ
};

enum class VS_ID : uint16_t
{
	Basic = 0,
};

enum class PS_ID : uint16_t
{
	Basic = 0,
	BBSceneEffect = 1,
};

enum class SHADER_DEFINE : uint64_t
{
	NONE = 0,

	// ---VS Defines (lower 32 bits)---

	// ---PS Defines (upper 32 bits)---
	PS_USE_MASK = 1ull << 32,
	PS_MULTIPLY_ALPHA_CONTROL = 1ull << 33,
	PS_USE_LIGHTING = 1ull << 34,
	PS_OUTLINE_RED = 1ull << 35,
	PS_OUTLINE_BLUE = 1ull << 36,
};
static inline SHADER_DEFINE operator|(SHADER_DEFINE a, SHADER_DEFINE b)
{
	return static_cast<SHADER_DEFINE>(static_cast<uint64_t>(a) | static_cast<uint64_t>(b));
}
constexpr bool Has(uint64_t m, SHADER_DEFINE f)
{
	return (m & static_cast<uint64_t>(f)) != 0;
}
constexpr uint64_t VS_DEFINE_MASK = 0x00000000FFFFFFFFull;
constexpr uint64_t PS_DEFINE_MASK = 0xFFFFFFFF00000000ull;

//ブレンドモード
enum BLEND_MODE
{
	BLEND_OPAQUE,
	BLEND_ALPHA,
	BLEND_ADD_ALPHA,
	BLEND_ADD,
	BLEND_MULTIPLY,
	BLEND_PREMULTIPLY,
};

//深度ステンシルモード
enum DEPTH_MODE
{
	DEPTH_DISABLE,
	DEPTH_TEST_WRITE,
	DEPTH_TEST_NO_WRITE,
};

enum CULL_MODE
{
	CULL_NONE,
	CULL_FRONT,
	CULL_BACK,
};

//パイプラインステートオブジェクトキー構造体
struct PSOKey
{
	VS_ID vsEntry = VS_ID::Basic;			//頂点シェーダーエントリポイント
	PS_ID psEntry = PS_ID::Basic;			//ピクセルシェーダーエントリポイント
	BLEND_MODE  blend = BLEND_OPAQUE;		//ブレンドモード
	DEPTH_MODE  depth = DEPTH_TEST_WRITE;	//深度ステンシルモード
	CULL_MODE  cull = CULL_NONE;			//カリングモード
	uint64_t defines = 0;

	//等価演算子オーバーロード
	bool operator == (const PSOKey& other) const
	{
		return vsEntry == other.vsEntry &&
			psEntry == other.psEntry &&
			defines == other.defines &&
			blend == other.blend &&
			depth == other.depth &&
			cull == other.cull;
	}
	bool operator != (const PSOKey& other) const
	{
		return !(*this == other);
	}

	PSOKey WithLighting() const {
		PSOKey k = *this;
		k.defines |= static_cast<uint64_t>(SHADER_DEFINE::PS_USE_LIGHTING);
		return k;
	}

	PSOKey AddDefines(std::initializer_list<SHADER_DEFINE> defines) const {
		PSOKey k = *this;
		for (auto d : defines) {
			k.defines |= static_cast<uint64_t>(d);
		}
		return k;
	}
};

inline constexpr PSOKey PSO_KEY_OPAQUE { VS_ID::Basic, PS_ID::Basic, BLEND_OPAQUE, DEPTH_TEST_WRITE, CULL_NONE, 0 };
inline constexpr PSOKey PSO_KEY_TRANSPARENT { VS_ID::Basic, PS_ID::Basic, BLEND_ALPHA, DEPTH_TEST_NO_WRITE, CULL_NONE, 0 };
inline constexpr PSOKey PSO_KEY_MASKED { VS_ID::Basic, PS_ID::Basic, BLEND_OPAQUE, DEPTH_TEST_WRITE, CULL_NONE, static_cast<uint64_t>(SHADER_DEFINE::PS_USE_MASK) };
inline constexpr PSOKey PSO_KEY_OUTLINE_RED { VS_ID::Basic, PS_ID::Basic, BLEND_OPAQUE, DEPTH_TEST_WRITE, CULL_NONE, static_cast<uint64_t>(SHADER_DEFINE::PS_OUTLINE_RED) };
inline constexpr PSOKey PSO_KEY_OUTLINE_BLUE { VS_ID::Basic, PS_ID::Basic, BLEND_OPAQUE, DEPTH_TEST_WRITE, CULL_NONE, static_cast<uint64_t>(SHADER_DEFINE::PS_OUTLINE_BLUE) };
inline constexpr PSOKey PSO_KEY_ADDITIVE { VS_ID::Basic, PS_ID::Basic, BLEND_ADD_ALPHA, DEPTH_TEST_NO_WRITE, CULL_NONE, 0 };
inline constexpr PSOKey PSO_KEY_MULTIPLY { VS_ID::Basic, PS_ID::Basic, BLEND_MULTIPLY, DEPTH_TEST_NO_WRITE, CULL_NONE, static_cast<uint64_t>(SHADER_DEFINE::PS_MULTIPLY_ALPHA_CONTROL) };

//ハッシュ関数オーバーロード
struct PSOKeyHash
{
	size_t operator()(const PSOKey& k) const noexcept
	{
		size_t h = std::hash<VS_ID>{}(k.vsEntry);
		auto hc = [&](size_t v) { h ^= v + 0x9e3779b97f4a7c15ULL + (h << 6) + (h >> 2); };
		hc(std::hash<PS_ID>{}(k.psEntry));
		hc(std::hash<int>{}(static_cast<int>(k.blend)));
		hc(std::hash<int>{}(static_cast<int>(k.depth)));
		hc(std::hash<int>{}(static_cast<int>(k.cull)));
		hc(std::hash<uint64_t>{}(k.defines));
		return h;
	}
};

enum RENDER_QUEUE
{
	RENDER_QUEUE_INVALID = -1,
	RENDER_QUEUE_OPAQUE = 0,
	RENDER_QUEUE_TRANSPARENT = 2,
};

static inline RENDER_QUEUE GetRenderQueueFromBlendMode(BLEND_MODE blendMode)
{
	return (blendMode == BLEND_OPAQUE) ? RENDER_QUEUE_OPAQUE : RENDER_QUEUE_TRANSPARENT;
}

//=======================================================================================================
//描画情報構造体群
//=======================================================================================================
//共通描画記述構造体
struct CommonRenderDesc
{
	MeshGPU* pMeshGPU = nullptr;							//メッシュデータ
	uint32_t srvIndex = UINT32_MAX;							//SRVインデックス(テクスチャ)
	DirectX::XMFLOAT4 color = { 1,1,1,1 };					//表示色
	DirectX::XMFLOAT4 uvRect{ 0.0f, 0.0f, 1.0f, 1.0f };		//UV矩形
	PSOKey psoKey{};										//パイプラインステートオブジェクトキー
	float sortDepth = 0.0f;									//ソート用深度
	RENDER_QUEUE renderQueue = RENDER_QUEUE_INVALID;		//レンダリングキュー
};

// Render information structure for world space
struct WorldRenderInfo
{
	CommonRenderDesc common{};				// Common render description structure
	DirectX::XMMATRIX world = {};			// World matrix
	UINT startIndex = 0;					// Start index
	INT  baseVertex = 0;					// Base vertex
	DirectX::XMFLOAT3 position{};			// Position
	DirectX::XMFLOAT3 scale{};				// Scale
	bool lightingEnabled = true;			// Lighting enabled flag
	BILLBOARD_TYPE billboardType
		= BILLBOARD_TYPE::BILLBOARD_NONE;	// Billboard type

	NodeAnimationAsset* pNodeAnimAsset = nullptr;	// Pointer to node animation asset
};

//描画情報構造体配列型
using WorldRenderModel = std::vector<WorldRenderInfo>;

//=======================================================================================================
//メッシュ・モデルデータ構造体
//=======================================================================================================
//メッシュデータ構造体
struct Mesh
{
	std::vector<Vertex> vertices;		//頂点データ配列
	size_t vertexCount = 0;				//頂点数
	std::vector<uint32_t> indices;		//インデックスデータ配列
	size_t indexCount = 0;				//インデックス数
	std::wstring texPath;				//テクスチャのファイル名
	DirectX::XMFLOAT4 materialColor		//材質色(RGBA)
	{
		1.0f,	//拡散反射色R
		1.0f,	//拡散反射色G
		1.0f,	//拡散反射色B
		1.0f	//拡散反射色A
	};
	NodeAnimationAsset nodeAnimAsset{};	// ノードアニメーション資産
};

// Bone data structure
struct Bone
{
	std::wstring name;				// Bone name
	int parentIndex = -1;			// Parent bone index (-1 if root)
	DirectX::XMMATRIX offset;		// Offset matrix
	int nodeIndex = -1;				// Node index in the model's node hierarchy
};

// Skeleton data structure
struct Skeleton
{
	std::vector<Bone> bones;						// Bone array
	std::unordered_map<std::wstring, int> boneMap;	// Map from bone name to index
};

struct AnimationClip
{
	std::wstring name;	// Animation clip name
	float duration;		// Duration in seconds
	float ticksPerSecond; // Ticks per second
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
inline constexpr uint32_t CubeIndices[42] =
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
	// -Y
	20,21,22,  20,22,23		//三角形11、12
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
	std::vector<WorldRenderInfo>& infos,	//入力元描画情報配列
	std::vector<WorldRenderInfo>& out	//出力先描画情報配列
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
	std::vector<WorldRenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	MESH_TYPE mType,						//メッシュタイプ
	PSOKey psoKey,							//ブレンドモード
	const wchar_t* path,					//モデルデータ又はテクスチャファイルのパス
	bool lightEneble = true,				//ライト有効or無効
	BILLBOARD_TYPE bType = BILLBOARD_NONE,	//ビルボードタイプ
	bool inverseU = false,					//Uを反転するかどうか(モデルデータの場合のみ有効)
	bool inverseV = false					//Vを反転するかどうか(モデルデータの場合のみ有効)
);

//FBXファイルから描画情報を作成する関数
void CreateRenderInfoFromFBX(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	std::vector<WorldRenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	PSOKey psoKey,							//ブレンドモード
	const wchar_t* path,					//モデルファイルのパス
	bool lightEneble,						//ライト有効or無効
	BILLBOARD_TYPE bType = BILLBOARD_NONE,	//ビルボードタイプ
	bool inverseU = false,					//Uを反転するかどうか
	bool inverseV = false					//Vを反転するかどうか
);

//デフォルトのメッシュデータから描画情報を作成する関数
void CreateRenderInfoFromDefaultMesh(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	std::vector<WorldRenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	MESH_TYPE type,							//メッシュタイプ
	PSOKey psoKey,							//ブレンドモード
	const wchar_t* path,					//テクスチャのファイル名
	bool lightEneble,						//ライト有効or無効
	BILLBOARD_TYPE bType = BILLBOARD_NONE	//ビルボードタイプ
);

//メッシュデータから描画情報を構築する関数
CommonRenderDesc CreateRenderInfoFromMeshData(
	TextureManager& textureManager,			//テクスチャマネージャへの参照
	MeshManager& meshManager,				//メッシュマネージャへの参照
	Mesh& mesh,					//メッシュデータ構造体への参照
	PSOKey psoKey,							//ブレンドモード
	BILLBOARD_TYPE bType = BILLBOARD_NONE	//ビルボードタイプ
);

//描画情報配列とジオメトリ情報から提出用描画情報配列を構築する関数
WorldRenderModel BuildRenderInfoForSubmit(
	const WorldRenderModel& input,
	MESH_TYPE meshType = MESH_TYPE::QUAD,
	const DirectX::XMFLOAT3& position = { 0.0f, 0.0f, 0.0f },
	const DirectX::XMFLOAT3& scale = { 1.0f, 1.0f, 1.0f },
	const DirectX::XMFLOAT3& rotation = { 0.0f, 0.0f, 0.0f },
	const DirectX::XMFLOAT4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
	const TexSplitInfo& texSplitInfo = {}
);