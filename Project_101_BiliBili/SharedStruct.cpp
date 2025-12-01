#include "SharedStruct.h"
#include <cmath>
#include "AssimpLoader.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "TextureManager.h"
#include "MeshManager.h"
#include "Collider.h"
#include "ObjectBase.h"

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

//描画情報構造体を作成する関数
void RenderData::CreateRenderInfo(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,		//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	MeshData::MESH_TYPE type,		//メッシュタイプ
	BLEND_MODE mode,				//ブレンドモード
	const wchar_t* path,			//モデルデータ又はテクスチャファイルのパス
	bool inverseU,					//Uを反転するかどうか(モデルデータの場合のみ有効)
	bool inverseV					//Vを反転するかどうか(モデルデータの場合のみ有効
)
{
	//メッシュタイプに応じて描画情報構造体を作成
	if(type == MeshData::IMPORT)
	{//インポートモデルの場合
		CreateRenderInfoFromFBX(	//FBXファイルから描画情報を作成
			textureManager,	//テクスチャマネージャへの参照
			meshManager,	//メッシュマネージャへの参照
			pInfo,			//描画情報構造体配列へのポインタ
			mode,			//ブレンドモード
			path,			//モデルファイルのパス
			inverseU,		//Uを反転するかどうか
			inverseV		//Vを反転するかどうか
		);
	}
	else
	{//デフォルトのメッシュデータの場合
		CreateRenderInfoFromDefaultMesh(	//デフォルトのメッシュデータから描画情報を作成
			textureManager,	//テクスチャマネージャへの参照
			meshManager,	//メッシュマネージャへの参照
			pInfo,			//描画情報構造体配列へのポインタ
			type,			//メッシュタイプ
			mode,			//ブレンドモード
			path			//テクスチャのファイル名
		);
	}
}

//FBXファイルから描画情報を作成する関数
void RenderData::CreateRenderInfoFromFBX(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,		//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	BLEND_MODE mode,				//ブレンドモード
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
		RenderInfo info = CreateRenderInfoFromMeshData(	//描画情報構造体の生成
			textureManager,	//テクスチャマネージャへの参照
			meshManager,	//メッシュマネージャへの参照
			mesh,			//メッシュデータ
			mode			//ブレンドモード
		);
		pInfo->push_back(info);	//配列に格納
	}
}

//デフォルトのメッシュデータから描画情報を作成する関数
void RenderData::CreateRenderInfoFromDefaultMesh(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,		//メッシュマネージャへの参照
	std::vector<RenderInfo>* pInfo,	//描画情報構造体配列へのポインタ
	MeshData::MESH_TYPE type,		//メッシュタイプ
	BLEND_MODE mode,				//ブレンドモード
	const wchar_t* path				//テクスチャのファイル名
)
{
	MeshData::Model model;	//モデルデータ構造体
	model = MeshData::GetModel(type);	//メッシュタイプに応じたメッシュデータを取得

	//メッシュタイプに応じたメッシュデータを取得して描画情報を作成
	for (auto& mesh : model.meshes)
	{
		mesh.texPath = path;	//テクスチャのファイル名を設定
		RenderInfo info = CreateRenderInfoFromMeshData(	//描画情報構造体の生成
			textureManager,		//テクスチャマネージャへの参照
			meshManager,		//メッシュマネージャへの参照
			mesh,				//メッシュデータ
			mode			//ブレンドモード
		);
		pInfo->push_back(info);	//配列に格納
	}
}

//メッシュデータから描画情報を構築する関数
RenderData::RenderInfo RenderData::CreateRenderInfoFromMeshData(
	TextureManager& textureManager,	//テクスチャマネージャへの参照
	MeshManager& meshManager,		//メッシュマネージャへの参照
	MeshData::Mesh& mesh,			//メッシュデータ
	BLEND_MODE mode				//ブレンドモード
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
	info.blendMode = mode;							//ブレンドモードを設定

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

MeshData::Model MeshData::MakeQuadModel()
{
	Mesh mesh;		//メッシュデータ構造体
	Model model;	//モデルデータ構造体

	//頂点データの設定
	mesh.vertices.assign(
		std::begin(QuadVertices),	//頂点データ配列の先頭アドレス
		std::end(QuadVertices)		//頂点データ配列の終端アドレス
	);

	//インデックスデータの設定
	mesh.indices.assign(
		std::begin(QuadIndices),	//インデックスデータ配列の先頭アドレス
		std::end(QuadIndices)		//インデックスデータ配列の終端アドレス
	);

	//頂点数の設定
	mesh.vertexCount = mesh.vertices.size();

	//インデックス数の設定
	mesh.indexCount = mesh.indices.size();

	//メッシュデータをモデルデータに追加
	model.meshes.push_back(mesh);

	return model;	//メッシュデータ構造体を返す

}

//立方体のメッシュデータ作成関数
MeshData::Model MeshData::MakeCubeModel()
{
	{
		Mesh mesh;		//メッシュデータ構造体
		Model model;	//モデルデータ構造体

		//頂点データの設定
		mesh.vertices.assign(
			std::begin(CubeVertices),	//頂点データ配列の先頭アドレス
			std::end(CubeVertices)		//頂点データ配列の終端アドレス
		);

		//インデックスデータの設定
		mesh.indices.assign(
			std::begin(CubeIndices),	//インデックスデータ配列の先頭アドレス
			std::end(CubeIndices)		//インデックスデータ配列の終端アドレス
		);

		//頂点数の設定
		mesh.vertexCount = mesh.vertices.size();

		//インデックス数の設定
		mesh.indexCount = mesh.indices.size();

		//メッシュデータをモデルデータに追加
		model.meshes.push_back(mesh);

		return model;	//メッシュデータ構造体を返す
	}

}

//球体のメッシュデータ作成関数
MeshData::Model MeshData::MakeSphereModel(int slices, int stacks)
{
	Mesh mesh;		//メッシュデータ構造体
	Model model;	//モデルデータ構造体

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

	model.meshes.push_back(mesh);	//メッシュデータをモデルデータに追加

	return model;	//メッシュデータ構造体を返す
}

//カプセルのメッシュデータ作成関数
MeshData::Model MeshData::MakeCapsuleModel(int slices, int stacks)
{
	//引数の補正
	if (slices < 3) slices = 3;	//スライスは最低3
	if (stacks < 4) stacks = 4;	//スタックは最低4

	//必要な構造体の生成
	Mesh cylinder;				//シリンダー部分のメッシュデータ構造体
	Mesh halfSphereTop;			//上半球の部分のメッシュデータ構造体
	Mesh halfSphereBottom;		//下半球部分のメッシュデータ構造体
	Model model;	//モデルデータ構造体

	//単位カプセルのパラメータ
	const float radius = 0.5f;		//半径
	const float halfHeight = 0.5f;	//半分の高さ

	//スタック数の内訳計算
	const int hemiStacks = stacks / 4;				//半球のスタック数
	int cylinderStacks = stacks - hemiStacks * 2;	//円柱のスタック数
	if (cylinderStacks < 1) cylinderStacks = 1;		//円柱のスタック数は最低1にする

	//頂点・インデックスデータ配列の予約
	const int ringStride = slices + 1;					//リングあたりの頂点数	
	const int cylinderRingCount = cylinderStacks + 1;	//円柱部分のリング数
	const int topHemiRingCount = hemiStacks + 1;		//上半球部分のリング数
	const int bottomHemiRingCount = hemiStacks + 1;		//下半球部分のリング数

	int estimatedVertexCount =							//頂点数の見積もり
		(cylinderRingCount + topHemiRingCount + bottomHemiRingCount) * ringStride;

	cylinder.vertices.reserve(estimatedVertexCount);			//頂点データ配列の予約
	cylinder.indices.reserve(estimatedVertexCount * 6);			//インデックスデータ配列の予約
	halfSphereTop.vertices.reserve(estimatedVertexCount);		//頂点データ配列の予約
	halfSphereTop.indices.reserve(estimatedVertexCount * 6);	//インデックスデータ配列の予約
	halfSphereBottom.vertices.reserve(estimatedVertexCount);	//頂点データ配列の予約
	halfSphereBottom.indices.reserve(estimatedVertexCount * 6);//インデックスデータ配列の予約

	//頂点データの作成
	//シリンダ部分の頂点データ作成
	for (int stack = 0; stack <= cylinderStacks; stack++)
	{
		float t = static_cast<float>(stack) / static_cast<float>(cylinderStacks);	//スタック比率
		float y = -halfHeight + t * (halfHeight * 2.0f);							//Y座標

		for (int slice = 0; slice <= slices; slice++)
		{
			float u = static_cast<float>(slice) / static_cast<float>(slices);	//U座標
			float theta = u * XM_2PI;											//経度角

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

			cylinder.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

	//上半球部分の頂点データ作成
	for (int stack = 0; stack <= hemiStacks; stack++)
	{
		float v = static_cast<float>(stack) / static_cast<float>(hemiStacks);	//V座標
		float phi = v * XM_PIDIV2;												//緯度角

		float sinPhi = sin(phi);	//sinφ
		float cosPhi = cos(phi);	//cosφ

		float y = radius * sinPhi;	//Y座標

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

			halfSphereTop.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

	//下半球部分の頂点データ作成
	for(int stack = 0; stack <= hemiStacks; stack++)
	{
		float v = static_cast<float>(stack) / static_cast<float>(hemiStacks);	//V座標
		float phi = v * XM_PIDIV2;												//緯度角

		float sinPhi = sin(phi);	//sinφ
		float cosPhi = cos(phi);	//cosφ

		float y = -sinPhi * radius;	//Y座標

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

			halfSphereBottom.vertices.push_back(vertex);	//頂点データを配列に追加
		}
	}

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

			cylinder.indices.push_back(i0);
			cylinder.indices.push_back(i1);
			cylinder.indices.push_back(i2);
			cylinder.indices.push_back(i1);
			cylinder.indices.push_back(i3);
			cylinder.indices.push_back(i2);
		}
	}

	//上半球部分のインデックスデータ作成
	const int ringCountTop = hemiStacks + 1;
	for (uint32_t stack = 0; stack < ringCountTop - 1; stack++)
	{
		for (uint32_t slice = 0; slice < slices; slice++)
		{
			uint32_t i0 = stack * ringStride + slice;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = i0 + ringStride;
			uint32_t i3 = i2 + 1;
			halfSphereTop.indices.push_back(i0);
			halfSphereTop.indices.push_back(i1);
			halfSphereTop.indices.push_back(i2);
			halfSphereTop.indices.push_back(i1);
			halfSphereTop.indices.push_back(i3);
			halfSphereTop.indices.push_back(i2);
		}
	}

	//下半球部分のインデックスデータ作成
	const int ringCountBottom = hemiStacks + 1;
	for (uint32_t stack = 0; stack < ringCountBottom - 1; stack++)
	{
		for (uint32_t slice = 0; slice < slices; slice++)
		{
			uint32_t i0 = stack * ringStride + slice;
			uint32_t i1 = i0 + 1;
			uint32_t i2 = i0 + ringStride;
			uint32_t i3 = i2 + 1;
			halfSphereBottom.indices.push_back(i0);
			halfSphereBottom.indices.push_back(i1);
			halfSphereBottom.indices.push_back(i2);
			halfSphereBottom.indices.push_back(i1);
			halfSphereBottom.indices.push_back(i3);
			halfSphereBottom.indices.push_back(i2);
		}
	}

	//頂点数とインデックス数の設定
	cylinder.vertexCount = cylinder.vertices.size();					//頂点数の設定
	cylinder.indexCount = cylinder.indices.size();						//インデックス数の設定
	halfSphereTop.vertexCount = halfSphereTop.vertices.size();			//頂点数の設定
	halfSphereTop.indexCount = halfSphereTop.indices.size();			//インデックス数の設定
	halfSphereBottom.vertexCount = halfSphereBottom.vertices.size();	//頂点数の設定
	halfSphereBottom.indexCount = halfSphereBottom.indices.size();		//インデックス数の設定

	//モデルデータにメッシュデータを追加
	model.meshes.push_back(cylinder);			//円柱部分のメッシュデータをモデルデータに追加
	model.meshes.push_back(halfSphereTop);		//上半球部分のメッシュデータをモデルデータに追加
	model.meshes.push_back(halfSphereBottom);	//下半球部分のメッシュデータをモデルデータに追加

	return model;	//メッシュデータ構造体を返す
}

//円柱のメッシュデータ作成関数
MeshData::Model MeshData::MakeCylinderModel(int slices, int stacks)
{
	Mesh mesh;	//シリンダー部分のメッシュデータ構造体
	Model model;	//モデルデータ構造体

	const float radius = 0.5f;		//半径
	const float halfHeight = 0.5f;	//半分の高さ

	const int hemiStacks = stacks / 4;				//半球のスタック数
	int cylinderStacks = stacks - hemiStacks * 2;	//円柱のスタック数
	if (cylinderStacks < 1) cylinderStacks = 1;		//円柱のスタック数は最低1にする

	const int ringStride = slices + 1; //リングあたりの頂点数	

	const int cylinderRingCount = cylinderStacks + 1;	//円柱部分のリング数

	int estimatedVertexCount = cylinderRingCount * ringStride;	//頂点数の見積もり

	mesh.vertices.reserve(estimatedVertexCount);		//頂点データ配列の予約
	mesh.indices.reserve(estimatedVertexCount * 6);	//インデックスデータ配列の予約

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

	//インデックスデータの作成
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

	//頂点数とインデックス数の設定
	mesh.vertexCount = mesh.vertices.size();					//頂点数の設定
	mesh.indexCount = mesh.indices.size();						//インデックス数の設定

	model.meshes.push_back(mesh);			//円柱部分のメッシュデータをモデルデータに追加

	return model;	//メッシュデータ構造体を返す
}

//カプセルの描画情報追加関数
void MeshData::AppendCapsuleRenderInfos(
	const CapsuleVisualDesc& desc,				//カプセル描画情報記述子
	const DirectX::XMFLOAT3& position,			//位置
	const DirectX::XMFLOAT3& scale,				//スケール
	const DirectX::XMFLOAT3& rotEuler,			//回転Euler角
	const DirectX::XMFLOAT4& color,				//色
	std::vector<RenderData::RenderInfo>& infos,	//カプセルの実体
	std::vector<RenderData::RenderInfo>& out	//出力先描画情報配列
)
{
	using namespace DirectX;

	//コライダースケールの取得
	const float sx = fabs(scale.x);
	const float sy = fabs(scale.y);
	const float sz = fabs(scale.z);

	//パラメータ計算
	const float diamiter = (std::max)(sx, sz);					//直径
	const float radius = diamiter * 0.5f;						//半径
	const float cylHeight = (std::max)(sy - diamiter, 0.0f);	//円柱部分の高さ
	const float halfHeight = cylHeight * 0.5f;					//円柱部分の半分の高さ

	//各種行列の計算
	//スケーリング行列
	const XMMATRIX cylS = XMMatrixScaling(diamiter, cylHeight, diamiter);	//円柱部分のスケーリング行列
	const XMMATRIX sphS = XMMatrixScaling(diamiter, diamiter, diamiter);	//半球部分のスケーリング行列

	//回転・平行移動行列
	const XMMATRIX R = XMMatrixRotationRollPitchYaw(						
		XMConvertToRadians(rotEuler.x),
		XMConvertToRadians(rotEuler.y), 
		XMConvertToRadians(rotEuler.z)
	);
	const XMMATRIX T = XMMatrixTranslation(position.x, position.y, position.z);

	const XMMATRIX topT = XMMatrixTranslation(0, +halfHeight, 0);	//上半球の平行移動行列(シリンダーの上端に移動)
	const XMMATRIX botT = XMMatrixTranslation(0, -halfHeight, 0);	//下半球の平行移動行列(シリンダーの下端に移動)

	//ワールド行列の計算
	const XMMATRIX cylW = cylS * R * T;
	const XMMATRIX topW = sphS * topT * R * T;
	const XMMATRIX botW = sphS * botT * R * T;

	//描画情報構造体配列の作成
	out.clear();				//出力先配列のクリア
	out.resize(infos.size());	//出力先配列のリサイズ
	for (size_t i = 0; i < infos.size(); i++)
	{//描画情報構造体のコピー
		out[i] = infos[i];
	}

	//各描画情報構造体のワールド行列と色の設定
	//円柱部分
	out[0].world = cylW;
	out[0].color = color;
	//上半球部分
	out[1].world = topW;
	out[1].color = color;
	//下半球部分
	out[2].world = botW;
	out[2].color = color;
}

//コリジョンレイヤーをビットマスクに変換する関数
CollisionData::LayerMask CollisionData::LayerToBit(COLLISION_LAYER layer)
{
	return static_cast<LayerMask>(1 << static_cast<uint32_t>(layer));
}

//コリジョンレイヤーからレイヤーマスクを取得する関数
CollisionData::LayerMask CollisionData::GetLayerMask(COLLISION_LAYER layer)
{
	switch (layer)
	{//コリジョンレイヤーごとに当たり判定を行うレイヤーマスクを設定
	case CollisionData::COLLISION_LAYER::DEFAULT:
		break;
	case CollisionData::COLLISION_LAYER::PLAYER:
		return MakeMask({
			COLLISION_LAYER::WALL,				//壁レイヤー
			COLLISION_LAYER::WALLPASS,			//弾貫通壁レイヤー
			COLLISION_LAYER::GROUND,			//地面レイヤー
			COLLISION_LAYER::BULLET,			//弾レイヤー
			COLLISION_LAYER::BB_LINE,			//ビリビリライン
			COLLISION_LAYER::BB_ELECTRICITY,	//ビリビリ電流
			});
		break;
	case CollisionData::COLLISION_LAYER::WALL:
		return MakeMask({
			COLLISION_LAYER::PLAYER,			//プレイヤーレイヤー
			COLLISION_LAYER::BULLET,			//弾レイヤー
			COLLISION_LAYER::BB_LINE,			//ビリビリライン
			COLLISION_LAYER::BB_ELECTRICITY,	//ビリビリ電流

			});
		break;
	case CollisionData::COLLISION_LAYER::WALLPASS:
		return MakeMask({
			COLLISION_LAYER::PLAYER,			//プレイヤーレイヤー
			COLLISION_LAYER::BB_LINE,			//ビリビリライン
			COLLISION_LAYER::BB_ELECTRICITY,	//ビリビリ電流
			});
		break;
	case CollisionData::COLLISION_LAYER::GROUND:
		return MakeMask({
			COLLISION_LAYER::PLAYER,	//プレイヤーレイヤー
			COLLISION_LAYER::BULLET		//弾レイヤー
			});
		break;
	case CollisionData::COLLISION_LAYER::BULLET:
		return MakeMask({
			COLLISION_LAYER::PLAYER,	//プレイヤーレイヤー
			COLLISION_LAYER::WALL,		//壁レイヤー
			COLLISION_LAYER::GROUND		//地面レイヤー
			});
		break;
	case CollisionData::COLLISION_LAYER::BB_LINE:
		return MakeMask({
			COLLISION_LAYER::PLAYER,	//プレイヤーレイヤー
			COLLISION_LAYER::WALL,		//壁レイヤー
			COLLISION_LAYER::WALLPASS,	//壁レイヤー
			});
		break;
	case CollisionData::COLLISION_LAYER::BB_ELECTRICITY:
		return MakeMask({
			COLLISION_LAYER::PLAYER,	//プレイヤーレイヤー
			COLLISION_LAYER::WALL,		//壁レイヤー
			COLLISION_LAYER::WALLPASS,	//壁レイヤー
			});
		break;
	case CollisionData::COLLISION_LAYER::MAX_LAYER:
		return 0;
		break;
	default:
		break;
	}
}

//複数のコリジョンレイヤーからレイヤーマスクを作成する関数
CollisionData::LayerMask CollisionData::MakeMask(std::initializer_list<COLLISION_LAYER> layers)
{
	LayerMask mask = 0;	//レイヤーマスク
	for (auto layer : layers)
	{
		mask |= LayerToBit(layer);	//ビットマスクを合成
	}
	return mask;	//レイヤーマスクを返す
}

//貫入深さから押し出しベクトルを取得する関数
DirectX::XMFLOAT3 CollisionData::GetPushOutVector(
	std::vector<CollisionData::CollisionInfo>& infos,	//衝突情報配列
	const std::initializer_list<OBJECT_TAG>& tagList	//押し出しベクトルを計算する対象のタグリスト
)
{
	using namespace DirectX;

	XMFLOAT3 total{ 0,0,0 };	//最大押し出しベクトル
	float epsilon = 0.0001f;	//誤差許容値

	std::vector<CollisionData::CollisionInfo*> cands;	//衝突情報配列をループ

	for (auto& info : infos)
	{
		//衝突終了は無視
		if (info.state == CollisionData::COLLISION_STATE::COLLISION_EXIT) continue;

		OBJECT_TAG opponentTag = info.opponent->GetOwner()->GetTag();	//衝突相手のタグ取得

		//衝突相手のタグがリストに含まれているか確認
		if (std::find(tagList.begin(), tagList.end(), opponentTag) == tagList.end()) continue;

		cands.push_back(&info);	//候補リストに追加
	}

	auto begin = cands.begin();
	auto end = cands.end();

	//貫入深さの大きい順にソート
	std::sort(cands.begin(), cands.end(),
		[](const CollisionData::CollisionInfo* a, const CollisionData::CollisionInfo* b)
		{
			return LengthXMF3(a->penetrationDepth) > LengthXMF3(b->penetrationDepth);
		}
	);

	const int REPEAT_MAX = 5;	//最大繰り返し回数

	for (int i = 0; i < REPEAT_MAX; i++)
	{
		bool any = false;	//押し出しが発生したかどうか

		for (auto pInfo : cands)
		{
			XMFLOAT3 penetration = pInfo->penetrationDepth;	//貫入深さベクトル

			XMFLOAT3 dir = { -penetration.x, -penetration.y, -penetration.z };	//押し出し方向ベクトル
			float depth = LengthXMF3(dir);									//貫入深さ
			if (depth < epsilon) continue;									//誤差許容値以下なら無視

			//押し出しベクトルの正規化
			dir = Normalize(dir);

			float resolved = (std::max)(0.0f, Dot(total, dir));	//既に押し出された分
			float remain = depth - resolved;					//残りの押し出し分
			if (remain > epsilon)
			{//押し出しが発生する場合
				//押し出しベクトルの加算
				total.x += dir.x * remain;
				total.y += dir.y * remain;
				total.z += dir.z * remain;
				any = true;	//押し出しが発生したフラグを立てる
			}
		}

		if (!any) break;	//押し出しが発生しなかったら終了
	}

	return total;	//押し出しベクトルを返す
}
