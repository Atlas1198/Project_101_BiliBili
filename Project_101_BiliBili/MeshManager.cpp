#include "MeshManager.h"

//デストラクタ
MeshManager::~MeshManager()
{
	for (auto mesh : m_Meshes)
	{
		delete mesh;
	}
	m_Meshes.clear();
}

//初期化
bool MeshManager::Initialize(ID3D12Device* pDevice)
{
	if (!pDevice)
	{
		OutputDebugStringA("[MeshManager] Direct3D device is unavailable\n");
		return false;
	}

	m_pDevice = pDevice;
	return true;
}

//メッシュを作成してリストに追加、メッシュへのポインタを返す
MeshGPU* MeshManager::CreateMesh(Mesh& src)
{
	if (!m_pDevice || src.vertices.empty() || src.indices.empty() ||
		src.vertexCount != src.vertices.size() || src.indexCount != src.indices.size())
	{
		OutputDebugStringA("[MeshManager] Invalid mesh data; GPU mesh was not created\n");
		return nullptr;
	}

	for (const uint32_t index : src.indices)
	{
		if (index >= src.vertices.size())
		{
			OutputDebugStringA("[MeshManager] Mesh contains an out-of-range index\n");
			return nullptr;
		}
	}

	MeshGPU* newMesh = new MeshGPU(m_pDevice, src);	//メッシュを作成
	if (!newMesh->IsValid())
	{
		OutputDebugStringA("[MeshManager] GPU mesh buffer creation failed\n");
		delete newMesh;
		return nullptr;
	}

	m_Meshes.push_back(newMesh);					//リストに追加
	return newMesh;									//メッシュへのポインタを返す
}
