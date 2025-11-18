#include "FieldManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

using namespace DirectX;

//コンストラクタ
FieldManager::FieldManager()
{
	//TODO: 地面と壁オブジェクトの生成
}

//デストラクタ
FieldManager::~FieldManager()
{
	//壁オブジェクトの解放
	for (auto& wall : m_pWalls)
	{
		delete wall;
		wall = nullptr;
	}
	m_pWalls.clear();

	//地面オブジェクトの解放
	for (auto& ground : m_pGrounds)
	{
		delete ground;
		ground = nullptr;
	}
	m_pGrounds.clear();
}

//初期化
void FieldManager::InitializeOverride(InputManager* pInputManager, TextureManager& textureManager, MeshManager& meshManager, CollisionManager& collisionManager)
{
}

//更新
void FieldManager::Update()
{
}

//描画要求をシーンに提出
void FieldManager::SubmitDraws(Renderer& renderer)
{
}

//衝突後処理
void FieldManager::ResolveCollisions()
{
}

//オブジェクトの描画情報生成
void FieldManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
}
