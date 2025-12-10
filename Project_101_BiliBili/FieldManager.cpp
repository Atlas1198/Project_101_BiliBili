#include <random>
#include "FieldManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"

using namespace DirectX;

//コンストラクタ
FieldManager::FieldManager()
{
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

	//弾貫通壁オブジェクトの解放
	for (auto& wallpass : m_pWallPasses)
	{
		delete wallpass;
		wallpass = nullptr;
	}
	m_pWallPasses.clear();

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
	std::random_device rd;               
	std::mt19937 gen(rd());              
	std::uniform_int_distribution<> dist(1, 3); 

	int Stagenum = dist(gen);
		
	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(-15.0f, 0.0f, 5.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(2.0f, 10.0f, 30.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(2.1f, 10.1f, 30.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);
	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(15.0f, 0.0f, 5.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(2.0f, 10.0f, 30.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(2.1f, 10.1f, 30.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);
	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(0.0f, 0.0f, 20.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(30.0f, 10.0f, 2.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(30.1f, 10.1f, 2.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);
	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(0.0f, 0.0f, -10.0f),	//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(30.0f, 10.0f, 2.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
			true,						//アクティブフラグ
			ColliderType::BOX,		//コライダータイプ
			XMFLOAT3(30.1f, 10.1f, 2.1f),	//コライダーボックスサイズ
			false						//コライダーのトリガーフラグ
		)
	);

	for(auto& wall : m_pWalls)
	{
		wall->SetDrawn(false);
	}

	m_pGrounds.push_back(
		new Ground(
			MESH_TYPE::QUAD,
			XMFLOAT3(0.0f, -5.0f, 5.0f),	//位置
			XMFLOAT3(90.0f, 0.0f, 0.0f),	//回転
			XMFLOAT3(30.0f, 30.0f, 1.0f),	//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
			true,							//アクティブフラグ
			ColliderType::BOX,				//コライダータイプ
			XMFLOAT3(30.0f, 30.0f, 1.0f),		//コライダーボックスサイズ
			false							//コライダーのトリガーフラグ
		)
	);

	switch (Stagenum)
	{
		case 1:	//ステージ1
			//-------------
			//左側
			//-------------
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-6.0f, -4.0f, 15.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(5.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(5.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-11.0f, -4.0f, 3.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(2.0f, 3.0f, 6.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(2.1f, 3.1f, 6.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-3.0f, -4.0f, 7.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 6.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 6.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-8.0f, -4.0f, -4.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(2.0f, 3.0f, 2.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(2.1f, 3.1f, 2.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWallPasses.push_back(
				new WallPass(
					MESH_TYPE::CUBE,
					XMFLOAT3(-4.0f, -4.0f, -4.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(6.0f, 3.0f, 2.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(6.1f, 3.1f, 2.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);


			//-------------
			//右側
			//-------------
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(3.0f, -4.0f, 3.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 6.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 6.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(8.0f, -4.0f, 14.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(2.0f, 3.0f, 2.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(2.1f, 3.1f, 2.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWallPasses.push_back(
				new WallPass(
					MESH_TYPE::CUBE,
					XMFLOAT3(4.0f, -4.0f, 14.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(6.0f, 3.0f, 2.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(6.1f, 3.1f, 2.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(11.0f, -4.0f, 7.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(2.0f, 3.0f, 6.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(2.1f, 3.1f, 6.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(6.0f, -4.0f, -5.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(5.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(5.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			
			break;
		case 2:	//ステージ2
			//-------------
			//左側
			//-------------
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-10.0f, -3.0f, 8.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(3.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(3.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-9.0f, -3.0f, 5.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 5.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 5.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-10.0f, -3.0f, 2.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(3.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(3.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-5.0f, -3.0f, 13.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(7.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(7.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-5.0f, -3.0f, 10.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 5.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 5.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWallPasses.push_back(
				new WallPass(
					MESH_TYPE::CUBE,
					XMFLOAT3(-5.0f, -3.0f, -5.5f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 7.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 7.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			//-------------
			//右側
			//-------------
			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(10.0f, -3.0f, 8.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(3.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(3.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(9.0f, -3.0f, 5.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 5.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 5.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(10.0f, -3.0f, 2.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(3.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(3.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(5.0f, -3.0f, -3.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(7.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(7.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(5.0f, -3.0f, 0.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 5.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 5.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			m_pWallPasses.push_back(
				new WallPass(
					MESH_TYPE::CUBE,
					XMFLOAT3(5.0f, -3.0f, 15.5f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 7.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 7.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			break;
		case 3: //ステージ3
			//-------------
			//左側
			//-------------
			m_pWallPasses.push_back(
				new WallPass(
					MESH_TYPE::CUBE,
					XMFLOAT3(-8.0f, -3.0f, -4.5f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 9.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 9.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(-5.0f, -3.0f, 17.5f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 3.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 3.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			//-------------
			//真ん中
			//-------------

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(0.0f, -3.0f, 5.0f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(8.0f, 3.0f, 1.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(8.1f, 3.1f, 1.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			//-------------
			//右側
			//-------------
			m_pWallPasses.push_back(
				new WallPass(
					MESH_TYPE::CUBE,
					XMFLOAT3(8.0f, -3.0f, 14.5f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 9.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 9.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);

			m_pWalls.push_back(
				new Wall(
					MESH_TYPE::CUBE,
					XMFLOAT3(5.0f, -3.0f, -7.5f),	//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
					XMFLOAT3(1.0f, 3.0f, 3.0f),	//スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
					true,						//アクティブフラグ
					ColliderType::BOX,		//コライダータイプ
					XMFLOAT3(1.1f, 3.1f, 3.1f),	//コライダーボックスサイズ
					false						//コライダーのトリガーフラグ
				)
			);
			break;

	}
	

	

	for (auto& wall : m_pWalls)
	{
		SubmitColliders(collisionManager, wall->GetCollider());
	}

	for (auto& wallpass : m_pWallPasses)
	{
		SubmitColliders(collisionManager, wallpass->GetCollider());
	}

	for(auto& ground : m_pGrounds)
	{
		SubmitColliders(collisionManager, ground->GetCollider());
	}
}

//更新
void FieldManager::UpdateOverride()
{
}

//描画要求をシーンに提出
void FieldManager::SubmitDrawsOverride(Renderer& renderer)
{
	//壁描画情報をシーンに提出
	for (auto& wall : m_pWalls)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*wall,			//ゲームオブジェクト配列の参照
			m_wallInfo		//壁描画情報
		);
	}

	//弾貫通壁描画情報をシーンに提出
	for (auto& wallpass : m_pWallPasses)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*wallpass,			//ゲームオブジェクト配列の参照
			m_wallPassInfo		//弾貫通壁描画情報
		);
	}

	//地面描画情報をシーンに提出
	for (auto& ground : m_pGrounds)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*ground,		//ゲームオブジェクト配列の参照
			m_groundInfo	//地面描画情報
		);
	}
}

//衝突後処理
void FieldManager::ResolveCollisionsOverride()
{
}

//終了
void FieldManager::FinalizeOverride()
{
	m_pWalls.clear();
	m_pWallPasses.clear();
	m_pGrounds.clear();

	m_wallInfo.clear();
	m_wallPassInfo.clear();
	m_groundInfo.clear();
}

//オブジェクトの描画情報生成
void FieldManager::PrepareRenderInfo(TextureManager& textureManager, MeshManager& meshManager)
{
	//描画情報生成関数を呼び出し、描画情報を作成
	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_wallInfo,					//描画情報構造体配列へのポインタ
		m_pWalls[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_OPAQUE,		//ブレンドモード
		wallTexPath							//テクスチャのファイル名
	);

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_wallPassInfo,					//描画情報構造体配列へのポインタ
		m_pWallPasses[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_OPAQUE,		//ブレンドモード
		wallPassTexPath							//テクスチャのファイル名
	);

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_groundInfo,					//描画情報構造体配列へのポインタ
		m_pGrounds[0]->GetMeshType(),	//メッシュタイプ
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		groundTexPath					//テクスチャのファイル名
	);

}
