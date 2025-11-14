#pragma once
#ifndef NOMINMAX
#define NOMINMAX
#endif
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
//#include <d3d12.h>
#include "d3dx12.h"
#include <DirectXMath.h>
#include "DirectXTex.h"
#include <vector>
#include <string>
#include "ComPtr.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "TextureManager.h"

// 頂点データ構造体
struct Vertex
{
	DirectX::XMFLOAT3 position;	//頂点の位置
	DirectX::XMFLOAT3 normal;	//頂点の法線
	DirectX::XMFLOAT2 uv;		//頂点のUV座標
	DirectX::XMFLOAT3 tangent;	//接空間
	DirectX::XMFLOAT4 color;	//頂点色

	static const D3D12_INPUT_LAYOUT_DESC InputLayout; //入力レイアウト

private:
	static const size_t InputLayoutCount = 5;								//入力レイアウトの要素数
	static const D3D12_INPUT_ELEMENT_DESC InputElements[InputLayoutCount];	//入力要素の配列
};

//変換行列構造体(256バイトアライメント)
struct alignas(256) Transform
{
	DirectX::XMMATRIX worldMatrix;	//ワールド行列
	DirectX::XMMATRIX viewMatrix;	//ビュー行列
	DirectX::XMMATRIX projMatrix;	//プロジェクション行列
	DirectX::XMFLOAT4 objectColor;	//オブジェクトの色RGBA
};

// カメラ情報構造体
struct CameraInfo
{
	DirectX::XMFLOAT3 position;	//カメラの位置
	DirectX::XMFLOAT3 target;	//カメラの注視点
	DirectX::XMFLOAT3 up;		//カメラの上方向ベクトル
	float fov;					//垂直視野角
	float aspectRatio;			//アスペクト比
	float nearZ;				//ニアクリップ距離
	float farZ;					//ファークリップ距離
};

//メッシュデータ用名前空間
namespace MeshData
{
	//メッシュデータ構造体
	struct Mesh
	{
		std::vector<Vertex> vertices;	//頂点データ配列
		size_t vertexCount = 0;			//頂点数
		std::vector<uint32_t> indices;	//インデックスデータ配列
		size_t indexCount = 0;			//インデックス数
		std::wstring texPath;			//テクスチャのファイル名
	};

	//メッシュタイプ列挙体
	enum MESH_TYPE
	{
		QUAD,				//四角平面
		CUBE,				//立方体
		SPHERE,				//球体
		CAPSULE,			//カプセル
		CYLINDER,			//円柱
		HALF_SPHERE_TOP,	//半球
		HALF_SPHERE_BOTTOM	//下半球
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
	inline Mesh MakeQuadMesh()
	{
		Mesh m;	//メッシュデータ構造体

		//頂点データの設定
		m.vertices.assign(
			std::begin(QuadVertices),	//頂点データ配列の先頭アドレス
			std::end(QuadVertices)		//頂点データ配列の終端アドレス
		);

		//インデックスデータの設定
		m.indices.assign(
			std::begin(QuadIndices),	//インデックスデータ配列の先頭アドレス
			std::end(QuadIndices)		//インデックスデータ配列の終端アドレス
		);

		//頂点数の設定
		m.vertexCount = m.vertices.size();

		//インデックス数の設定
		m.indexCount = m.indices.size();

		return m;	//メッシュデータ構造体を返す
	}


	//=======================
	//立方体
	//=======================
	//立方体の頂点データ(24頂点)
	inline constexpr Vertex CubeVertices[24]=
	{
		// +Z
		{{-0.5,  0.5,  0.5}, {0,0,1}, {0,0}, {1,0,0}, {1,1,1,1}},		//頂点0
		{{ 0.5,  0.5,  0.5}, {0,0,1}, {1,0}, {1,0,0}, {1,1,1,1}},		//頂点1
		{{ 0.5, -0.5,  0.5}, {0,0,1}, {1,1}, {1,0,0}, {1,1,1,1}},		//頂点2
		{{-0.5, -0.5,  0.5}, {0,0,1}, {0,1}, {1,0,0}, {1,1,1,1}},		//頂点3

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
	inline Mesh MakeCubeMesh()
	{
		Mesh m;	//メッシュデータ構造体

		//頂点データの設定
		m.vertices.assign(
			std::begin(CubeVertices),	//頂点データ配列の先頭アドレス
			std::end(CubeVertices)		//頂点データ配列の終端アドレス
		);

		//インデックスデータの設定
		m.indices.assign(
			std::begin(CubeIndices),	//インデックスデータ配列の先頭アドレス
			std::end(CubeIndices)		//インデックスデータ配列の終端アドレス
		);

		//頂点数の設定
		m.vertexCount = m.vertices.size();

		//インデックス数の設定
		m.indexCount = m.indices.size();

		return m;	//メッシュデータ構造体を返す
	}	

	//=======================
	//球体
	//=======================
	//球体のメッシュデータ作成関数
	Mesh MakeSphereMesh(int slice = 32, int stacks = 16);

	//=======================
	//カプセル
	//=======================
	//カプセルのメッシュデータ作成関数
	Mesh MakeCapsuleMesh(int slice = 32, int stacks = 16);

	//=======================
	//円柱
	//=======================
	//円柱のメッシュデータ作成関数
	Mesh MakeCylinderMesh(int slice = 32, int stacks = 16);

	//=======================
	//半球
	//=======================
	//半球のメッシュデータ作成関数
	Mesh MakeHalfSphereMesh(bool top, int slice = 32, int stacks = 16);

	//メッシュデータ取得関数
	inline Mesh GetMesh(MESH_TYPE type)
	{
		//メッシュタイプに応じたメッシュデータを返す
		switch (type) 
		{
		case QUAD: return MakeQuadMesh();		//四角平面
		case CUBE: return MakeCubeMesh();		//立方体
		case SPHERE: return MakeSphereMesh();	//球体
		case CAPSULE: return MakeCapsuleMesh();	//カプセル
		case CYLINDER: return {};				//円柱(未実装)
		case HALF_SPHERE_TOP: return {};		//半球(未実装)
		case HALF_SPHERE_BOTTOM: return {};		//下半球(未実装)
		default:   return {};					//その他
		}
	}
}

//前方宣言
class TextureManager;
class MeshManager;
class MeshGPU;

namespace RenderData
{
	//描画情報構造体
	struct RenderInfo
	{
		MeshGPU* pMeshGPU = nullptr;			//メッシュGPUデータへのポインタ
		DirectX::XMMATRIX world = {};			//ワールド行列
		UINT startIndex = 0;					//開始インデックス
		INT  baseVertex = 0;					//ベース頂点
		uint32_t srvIndex = UINT32_MAX;			//SRVインデックス
		DirectX::XMFLOAT4 color = { 1,1,1,1 };	//オブジェクトの色RGBA(デフォルトは白)
	};

	//メッシュデータから描画情報を構築する関数
	RenderInfo CreateRenderInfo(
		TextureManager& textureManager,
		MeshManager& meshManager,
		MeshData::Mesh& mesh
	);

	//FBXファイルから描画情報を作成する関数
	void CreateRenderInfoFromFBX(
		TextureManager& textureManager,	//テクスチャマネージャへの参照
		MeshManager& meshManager,			//メッシュマネージャへの参照
		std::vector<RenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
		const wchar_t* path,			//モデルファイルのパス
		bool inverseU = false,			//Uを反転するかどうか
		bool inverseV = false			//Vを反転するかどうか
	);

	//デフォルトのメッシュデータから描画情報を作成する関数
	void CreateRenderInfoFromDefaultMesh(
		TextureManager& textureManager,		//テクスチャマネージャへの参照
		MeshManager& meshManager,			//メッシュマネージャへの参照
		std::vector<RenderInfo>* pInfo,		//描画情報構造体配列へのポインタ
		MeshData::MESH_TYPE type,			//メッシュタイプ
		const wchar_t* path = L""			//テクスチャのファイル名
	);
}

//前方宣言
class Collider;

//衝突情報構造体
struct CollisionInfo
{
	Collider* opponent;					//衝突相手のコライダー
	DirectX::XMFLOAT3 contactPoint;		//衝突点
	DirectX::XMFLOAT3 contactNormal;	//衝突法線
	DirectX::XMFLOAT3 penetrationDepth;	//貫入深さ
};