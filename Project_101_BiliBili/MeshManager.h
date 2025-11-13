#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <vector>
#include "SharedStruct.h"
#include "MeshGPU.h"

//メッシュ管理クラス
class MeshManager
{
public:
	MeshManager() {};
	~MeshManager();

	void Initialize(ID3D12Device* pDevice);	//初期化

	MeshGPU* CreateMesh(MeshData::Mesh& src);	//メッシュを作成してリストに追加、メッシュへのポインタを返す

private:
	std::vector<MeshGPU*> m_Meshes;	//メッシュのリスト

	ID3D12Device* m_pDevice = nullptr;	//デバイスへのポインタ
};