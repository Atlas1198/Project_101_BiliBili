#include "SharedStruct.h"
#include <cmath>
#include "AssimpLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TextureManager.h"
#include "MeshManager.h"
#include "Collider.h"

using namespace DirectX;

//頂点データの入力要素配列の定義
const D3D12_INPUT_ELEMENT_DESC Vertex::InputElements[] =
{
	{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	{ "COLOR",    0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
};

//頂点データの入力レイアウトの定義
const D3D12_INPUT_LAYOUT_DESC Vertex::InputLayout =
{
	Vertex::InputElements,		// 入力要素配列のポインタ
	Vertex::InputLayoutCount	// 入力要素の数
};

//メッシュデータから描画情報を構築する関数
RenderData::RenderInfo RenderData::CreateRenderInfo(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,		//メッシュマネージャへの参照
	MeshData::Mesh& mesh			//メッシュデータ
)
{
	RenderInfo info{};	//描画情報構造体

	//メッシュデータが空の場合は処理を抜ける
	if (mesh.vertices.empty() || mesh.indices.empty()) return info;

	//メッシュGPUデータの作成と描画情報構造体への設定
	info.pMeshGPU = meshManager.CreateMesh(mesh);	//メッシュGPUデータの作成とポインタの取得
	info.startIndex = 0;							//開始インデックス
	info.baseVertex = 0;							//ベース頂点
	info.world = XMMatrixIdentity();				//ワールド行列を単位行列に設定
	info.color = XMFLOAT4(1, 1, 1, 1);				//オブジェクトの色を白に設定

	//テクスチャのSRVインデックスを取得
	if (!mesh.texPath.empty() && &textureManager)
	{//テクスチャのファイル名が設定されていて、テクスチャマネージャが有効な場合
		info.srvIndex = textureManager.LoadSrvFromFile(mesh.texPath);	//テクスチャのSRVインデックスを取得
	}
	else
	{//テクスチャのファイル名が設定されていない場合
		info.srvIndex = UINT32_MAX;	//SRVインデックスを無効に設定
	}

	return info;	//描画情報構造体を返す
}

//FBXファイルから描画情報を作成する関数
void RenderData::CreateRenderInfoFromFBX(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,			//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	const wchar_t* path,			//モデルファイルのパス
	bool inverseU,					//Uを反転するかどうか
	bool inverseV					//Vを反転するかどうか
)
{
	std::vector<MeshData::Mesh> meshes;	//メッシュデータ配列

	//モデルインポート設定構造体の生成
	ImportSettings importSetting =
	{
		path,		//ファイルパス
		meshes,		//メッシュデータ配列への参照
		inverseU,	//Uを反転するかどうか
		inverseV	//Vを反転するかどうか
	};

	//Assimpによるモデル読み込み
	AssimpLoader loader;	//Assimpローダー生成
	if (!loader.Load(importSetting))
	{
		return;
	}

	//メッシュごとに描画情報構造体を生成して配列に格納
	for (auto& mesh : meshes)
	{
		RenderInfo info = CreateRenderInfo(	//描画情報構造体の生成
			textureManager,	//テクスチャマネージャへの参照
			meshManager,	//メッシュマネージャへの参照
			mesh			//メッシュデータ
		);
		pInfo->push_back(info);				//配列に格納
	}
}

//デフォルトのメッシュデータから描画情報を作成する関数
void RenderData::CreateRenderInfoFromDefaultMesh(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,		//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	MeshData::MESH_TYPE type,			//メッシュタイプ
	const wchar_t* path				//テクスチャのファイル名
)
{
	std::vector<MeshData::Mesh> meshes;	//メッシュデータ配列

	if (type == MeshData::MESH_TYPE::CAPSULE)
	{//カプセルメッシュの場合
		//上下の半球と円柱を別々で読み込み
		meshes.push_back(MeshData::GetMesh(MeshData::HALF_SPHERE_TOP));		//上半球
		meshes.push_back(MeshData::GetMesh(MeshData::CYLINDER));			//円柱
		meshes.push_back(MeshData::GetMesh(MeshData::HALF_SPHERE_BOTTOM));	//下半球
	}
	else
	{
		meshes.push_back(MeshData::GetMesh(type));	//メッシュデータを配列に追加
	}

	//メッシュタイプに応じたメッシュデータを取得して描画情報を作成
	for (auto& mesh : meshes)
	{
		mesh.texPath = path;							//テクスチャのファイル名を設定
		RenderInfo info = CreateRenderInfo(				//描画情報構造体の生成
			textureManager,		//テクスチャマネージャへの参照
			meshManager,		//メッシュマネージャへの参照
			mesh				//メッシュデータ
		);
		pInfo->push_back(info);							//配列に格納
	}
}

//球体のメッシュデータ作成関数
MeshData::Mesh MeshData::MakeSphereMesh(int slices, int stacks)
{
	Mesh mesh; //メッシュデータ構造体

	//頂点データの作成
	for(int stack = 0; stack <= stacks; stack++)
	{//スタックループ
		float v = static_cast<float>(stack) / static_cast<float>(stacks);	//V座標
		float phi = (v - 0.5f) * XM_PI;							//緯度角

		float y = sin(phi);	 //Y座標
		float r = cos(phi);	 //半径

		for (int slice = 0; slice <= slices; slice++)
		{//スライスループ
			float u = static_cast<float>(slice) / static_cast<float>(slices);	//U座標
			float theta = u * XM_2PI;									//経度角

			float x = r * cos(theta);	//X座標
			float z = r * sin(theta);	//Z座標

			//頂点データの作成
			Vertex vertex{};
			vertex.position = XMFLOAT3(x * 0.5f, y * 0.5f, z * 0.5f);	//位置
			vertex.normal = XMFLOAT3(x, y, z);							//法線
			vertex.uv = XMFLOAT2(u, 1.0f - v);							//UV座標
			vertex.tangent = XMFLOAT3(-sin(theta), 0.0f, cos(theta));	//接空間
			vertex.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			//頂点色

			mesh.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

	//インデックスデータの作成
	int ringStride = slices + 1; //リングあたりの頂点数
	for (int stack = 0; stack < stacks; stack++)
	{//スタックループ
		for (int slice = 0; slice < slices; slice++)
		{//スライスループ
			//インデックスの計算
			uint32_t index0 = stack * ringStride + slice;
			uint32_t index1 = (stack + 1) * ringStride + slice;
			uint32_t index2 = (stack + 1) * ringStride + (slice + 1);
			uint32_t index3 = stack * ringStride + (slice + 1);

			//三角形の追加
			mesh.indices.push_back(index0);
			mesh.indices.push_back(index1);
			mesh.indices.push_back(index2);
			mesh.indices.push_back(index0);
			mesh.indices.push_back(index2);
			mesh.indices.push_back(index3);
		}
	}

	mesh.vertexCount = mesh.vertices.size();	//頂点数の設定
	mesh.indexCount = mesh.indices.size();		//インデックス数の設定

	return mesh;	//メッシュデータ構造体を返す
}

//カプセルのメッシュデータ作成関数
MeshData::Mesh MeshData::MakeCapsuleMesh(int slices, int stacks)
{
	Mesh mesh; //メッシュデータ構造体

	const float radius = 0.5f;		//半径
	const float halfHeight = 0.5f;	//半分の高さ

	const int hemiStacks = stacks / 4;				//半球のスタック数
	int cylinderStacks = stacks - hemiStacks * 2;	//円柱のスタック数
	if (cylinderStacks < 1) cylinderStacks = 1;		//円柱のスタック数は最低1にする

	const int ringStride = slices + 1; //リングあたりの頂点数	

	const int cylinderRingCount = cylinderStacks + 1;	//円柱部分のリング数
	const int topHemiRingCount = hemiStacks;			//上半球部分のリング数
	const int bottomHemiRingCount = hemiStacks;			//下半球部分のリング数

	int estimatedVertexCount =	//頂点数の見積もり
		(cylinderRingCount + topHemiRingCount + bottomHemiRingCount) * ringStride;

	mesh.vertices.reserve(estimatedVertexCount);		//頂点データ配列の予約
	mesh.indices.reserve(estimatedVertexCount * 6);		//インデックスデータ配列の予約

	//頂点データの作成
	//シリンダ部分の頂点データ作成
	for (int stack = 0; stack <= cylinderStacks; stack++)
	{
		float t = static_cast<float>(stack) / static_cast<float>(cylinderStacks);	//スタック比率
		float y = -halfHeight + t * (halfHeight * 2.0f);							//Y座標

		for (int slice = 0; slice <= slices; slice++)
		{
			float u = static_cast<float>(slice) / static_cast<float>(slices);	//U座標
			float theta = u * XM_2PI;									//経度角

		float x = radius * cos(theta);	//X座標
		float z = radius * sin(theta);	//Z座標

			Vertex vertex{};
			vertex.position = XMFLOAT3(x, y, z);						//位置
			vertex.normal = XMFLOAT3(x / radius, 0.0f, z / radius);		//法線
			vertex.uv = XMFLOAT2(u, t);									//UV座標

			float tx = -radius * sin(theta);	//接空間X
			float tz = radius * cos(theta);		//接空間Z
			vertex.tangent = XMFLOAT3(tx, 0.0f, tz);					//接空間

			vertex.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			//頂点色

			mesh.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

	const uint32_t cylinderVertexCount = static_cast<uint32_t>(mesh.vertices.size()); //円柱部分の頂点数

	//上半球部分の頂点データ作成
	const uint32_t topStartIndex = cylinderVertexCount;	//上半球部分の開始インデックス

	for (int stack = 0; stack <= hemiStacks; stack++)
	{
		float v = static_cast<float>(stack) / static_cast<float>(hemiStacks);	//V座標
		float phi = v * XM_PIDIV2;												//緯度角

		float sinPhi = sin(phi);	//sinφ
		float cosPhi = cos(phi);	//cosφ

		float yCenter = halfHeight;					//上半球の中心Y座標
		float y = halfHeight + radius * sinPhi;		//Y座標

		float ringR = cosPhi * radius;	//リングの半径

		for(int slice = 0; slice <= slices; slice++)
		{
			float u = static_cast<float>(slice) / static_cast<float>(slices);	//U座標
			float theta = u * XM_2PI;											//経度角

			float x = ringR * cos(theta);	//X座標
			float z = ringR * sin(theta);	//Z座標

			float nx = cosPhi * cos(theta);	//法線X
			float ny = sinPhi;				//法線Y
			float nz = cosPhi * sin(theta);	//法線Z

			Vertex vertex{};
			vertex.position = XMFLOAT3(x, y, z);				//位置
			vertex.normal = XMFLOAT3(nx, ny, nz);				//法線
			vertex.uv = XMFLOAT2(u, 1.0f - (v * 0.5f + 0.5f));	//UV座標

			float tx = -ringR * sin(theta);		//接空間X
			float tz = ringR * cos(theta);		//接空間Z
			vertex.tangent = XMFLOAT3(tx, 0.0f, tz);			//接空間

			vertex.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	//頂点色

			mesh.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

	//下半球部分の頂点データ作成
	const uint32_t bottomStartIndex = static_cast<uint32_t>(mesh.vertices.size());;	//下半球部分の開始インデックス

	for(int stack = 0; stack <= hemiStacks; stack++)
	{
		float v = static_cast<float>(stack) / static_cast<float>(hemiStacks);	//V座標
		float phi = v * XM_PIDIV2;												//緯度角

		float sinPhi = sin(phi);	//sinφ
		float cosPhi = cos(phi);	//cosφ

		float yCenter = -halfHeight;			//下半球の中心Y座標
		float y = yCenter - sinPhi * radius;	//Y座標

		float ringR = cosPhi * radius;	//リングの半径

		for (int slice = 0; slice <= slices; slice++)
		{
			float u = static_cast<float>(slice) / static_cast<float>(slices);	//U座標
			float theta = u * XM_2PI;											//経度角

			float x = ringR * cos(theta);	//X座標
			float z = ringR * sin(theta);	//Z座標

			float nx = cosPhi * cos(theta);	//法線X
			float ny = -sinPhi;				//法線Y
			float nz = cosPhi * sin(theta);	//法線Z

			Vertex vertex{};
			vertex.position = XMFLOAT3(x, y, z);				//位置
			vertex.normal = XMFLOAT3(nx, ny, nz);				//法線
			vertex.uv = XMFLOAT2(u, (v * 0.5f));				//UV座標

			float tx = -ringR * sin(theta);		//接空間X
			float tz = ringR * cos(theta);		//接空間Z
			vertex.tangent = XMFLOAT3(tx, 0.0f, tz);			//接空間

			vertex.color = XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);	//頂点色

			mesh.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

	const uint32_t bottomVertexCount = static_cast<uint32_t>(mesh.vertices.size()) - bottomStartIndex;	//下半球部分の頂点数

	//インデックスデータの作成
	//シリンダ部分のインデックスデータ作成
	for (uint32_t stack = 0; stack < cylinderStacks; stack++)
	{
		for (uint32_t slice = 0; slice < slices; slice++)
		{
			uint32_t i0 = stack * ringStride + slice;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = i0 + ringStride;
			uint32_t i3 = i2 + 1;

			mesh.indices.push_back(i0);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i2);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i3);
			mesh.indices.push_back(i2);
		}
	}

	//上半球部分のインデックスデータ作成
	const int ringCountTop = hemiStacks + 1;
	for (uint32_t stack = 0; stack < ringCountTop - 1; stack++)
	{
		for (uint32_t slice = 0; slice < slices; slice++)
		{
			uint32_t i0 = topStartIndex + stack * ringStride + slice;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = i0 + ringStride;
			uint32_t i3 = i2 + 1;
			mesh.indices.push_back(i0);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i2);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i3);
			mesh.indices.push_back(i2);
		}
	}

	//下半球部分のインデックスデータ作成
	const int ringCountBottom = hemiStacks;
	for (uint32_t stack = 0; stack < ringCountBottom - 1; stack++)
	{
		for (uint32_t slice = 0; slice < slices; slice++)
		{
			uint32_t i0 = bottomStartIndex + stack * ringStride + slice;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = i0 + ringStride;
			uint32_t i3 = i2 + 1;
			mesh.indices.push_back(i0);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i2);
			mesh.indices.push_back(i1);
			mesh.indices.push_back(i3);
			mesh.indices.push_back(i2);
		}
	}

	mesh.vertexCount = mesh.vertices.size();	//頂点数の設定
	mesh.indexCount = mesh.indices.size();		//インデックス数の設定

	return mesh;	//メッシュデータ構造体を返す
}

//円柱のメッシュデータ作成関数
MeshData::Mesh MeshData::MakeCylinderMesh(int slice, int stacks)
{
	return Mesh();
}

MeshData::Mesh MeshData::MakeHalfSphereMesh(bool top, int slice, int stacks)
{
	return Mesh();
}


