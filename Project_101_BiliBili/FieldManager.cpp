#include <random>
#include "FieldManager.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "SharedStruct.h"
#include "AssimpNodeTransformAnim.h"
#include "Debug.h"

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

	//弾貫通壁オブジェクトの解放
	for (auto& wallpass : m_pWallPasses)
	{
		delete wallpass;
		wallpass = nullptr;
	}
	m_pWallPasses.clear();

	//カーブ壁オブジェクトの解放
	for (auto& wallcurve : m_pWallCurves)
	{
		delete wallcurve;
		wallcurve = nullptr;
	}
	m_pWallCurves.clear();

	//地面オブジェクトの解放
	for (auto& spring : m_pSprings)
	{
		delete spring;
		spring = nullptr;
	}
	m_pSprings.clear();

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
	std::uniform_int_distribution<> dist(1, 5);

	int Stagenum = dist(gen);

	Stagenum = 1;
	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(-25.0f, 0.0f, 5.0f),//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
			XMFLOAT3(2.0f, 10.0f, 30.0f),//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
			true,						 //アクティブフラグ
			ColliderType::BOX			 //コライダータイプ
		)
	);
	m_pWalls.back()->GetColliderSet()->AddCollider(
		ColliderType::BOX,
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.1f, 10.1f, 30.1f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(25.0f, 0.0f, 5.0f), //位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
			XMFLOAT3(2.0f, 10.0f, 30.0f),//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
			true,						 //アクティブフラグ
			ColliderType::BOX			 //コライダータイプ
		)
	);
	m_pWalls.back()->GetColliderSet()->AddCollider(
		ColliderType::BOX,
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(2.1f, 10.1f, 30.1f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(0.0f, 0.0f, 19.5f), //位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
			XMFLOAT3(48.0f, 10.0f, 2.0f),//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
			true,						 //アクティブフラグ
			ColliderType::BOX			 //コライダータイプ
		)
	);
	m_pWalls.back()->GetColliderSet()->AddCollider(
		ColliderType::BOX,
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(48.1f, 10.1f, 2.1f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	m_pWalls.push_back(
		new Wall(
			MESH_TYPE::CUBE,
			XMFLOAT3(0.0f, 0.0f, -9.5f),//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
			XMFLOAT3(48.0f, 10.0f, 2.0f),//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
			true,						 //アクティブフラグ
			ColliderType::BOX			 //コライダータイプ
		)
	);
	m_pWalls.back()->GetColliderSet()->AddCollider(
		ColliderType::BOX,
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		XMFLOAT3(48.1f, 10.1f, 2.1f),
		XMFLOAT3(0.0f, 0.0f, 0.0f)
	);

	for (auto& wall : m_pWalls)
	{
		wall->SetDrawn(false);
	}

	m_pGrounds.push_back(
		new Ground(
			MESH_TYPE::QUAD,
			XMFLOAT3(0.0f, -6.5f, 11.0f), //位置
			XMFLOAT3(0.0f, 0.0f, 0.0f), //回転
			XMFLOAT3(1.5f, 1.5f, 1.5f),//スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
			true,						 //アクティブフラグ
			ColliderType::BOX			 //コライダータイプ
		)
	);

	m_pWallCurves.push_back(
		new WallCurve(
			MESH_TYPE::IMPORT,
			XMFLOAT3(5.0f, -3.0f, 5.0f),  //位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
			XMFLOAT3(3.0f, 1.5f, 3.0f),	  //スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
			false,						  //アクティブフラグ
			ColliderType::BOX,			  //コライダータイプ
			XMFLOAT3(-1.5f, 1.5f, -2.5f),
			false,
			1.0f,
			true,
			3.0f,
			XMFLOAT3(0.0f, -4.5f, 5.0f),  //位置
			-90.0f						  //正面補正角
		)
	);

	m_pSprings.push_back(
		new Spring(
			MESH_TYPE::CUBE,
			XMFLOAT3(22.0f, -6.0f, 18.5f),//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
			XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
			true,						  //アクティブフラグ
			ColliderType::BOX			  //コライダータイプ
		)
	);
	m_pSprings.push_back(
		new Spring(
			MESH_TYPE::CUBE,
			XMFLOAT3(-22.0f, -6.0f, -7.5f),//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
			XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
			true,						  //アクティブフラグ
			ColliderType::BOX			  //コライダータイプ
		)
	);
	m_pSprings.push_back(
		new Spring(
			MESH_TYPE::CUBE,
			XMFLOAT3(22.0f, -6.0f, -7.5f),//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
			XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
			true,						  //アクティブフラグ
			ColliderType::BOX			  //コライダータイプ
		)
	);
	m_pSprings.push_back(
		new Spring(
			MESH_TYPE::CUBE,
			XMFLOAT3(-22.0f, -6.0f, 18.5f),//位置
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
			XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
			XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
			true,						  //アクティブフラグ
			ColliderType::BOX			  //コライダータイプ
		)
	);

	switch (Stagenum)
	{
	case 1:	//緑基盤
		//-------------
		//バネ
		//-------------

		m_pSprings.push_back(
				new Spring(
					MESH_TYPE::CUBE,
					XMFLOAT3(24.0f, -5.0f, 18.5f),//位置
					XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
					XMFLOAT3(2.5f, 2.5f, 2.5f),	  //スケール
					XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
					XMFLOAT3(-13.0f, -4.0f, 18.0f),	  //発射ターゲット（このスプリング固有）
					true,					  //アクティブフラグ
					ColliderType::BOX			  //コライダータイプ
				)
			);
		// 4地点ランダム	
		m_pSprings.back()->SetLaunchTargets({
			XMFLOAT3(1.0f, -4.0f, 12.0f),
			XMFLOAT3(0.0f, -4.0f, -1.0f),
			XMFLOAT3(-9.0f, -4.0f, 3.0f),
			XMFLOAT3(14.0f, -4.0f, -5.0f),
			}, true);	
		
		m_pSprings.push_back(
			new Spring(
				MESH_TYPE::CUBE,
				XMFLOAT3(-22.0f, -5.0f, -0.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(2.5f, 2.5f, 2.5f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				XMFLOAT3(-13.0f, -4.0f, 18.0f),	  //発射ターゲット（このスプリング固有）
				true,					  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		// 4地点ランダム
		m_pSprings.back()->SetLaunchTargets({
			XMFLOAT3(-14.0f, -4.0f, 16.0f),
			XMFLOAT3(7.0f, -4.0f, 2.0f),
			XMFLOAT3(-7.0f, -4.0f, -3.0f),
			XMFLOAT3(12.0f, -4.0f, 7.0f),
			}, true);

		//-------------
		//壁
		//-------------
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-10.0f, -5.0f, 13.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-12.5f, -5.0f, 13.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-15.0f, -5.0f, 13.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-19.0f, -5.0f, -3.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-19.0f, -5.0f, -0.5f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-19.0f, -5.0f, 2.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(10.0f, -5.0f, -3.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(12.5f, -5.0f, -3.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(15.0f, -5.0f, -3.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(19.0f, -5.0f, 13.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(19.0f, -5.0f, 10.5f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(19.0f, -5.0f, 8.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////

		//-------------
		//穴
		//-------------
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(4.0f, -5.5f, -2.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(1.5f, -5.5f, -2.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(7.0f, -5.5f, 10.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(7.0f, -5.5f, 7.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(7.0f, -5.5f, 5.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-4.0f, -5.5f, 12.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-1.5f, -5.5f, 12.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		///////////////////////////////////
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-7.0f, -5.5f, 5.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-7.0f, -5.5f, 2.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-7.0f, -5.5f, 0.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		break;
	case 2:	//ステージ2
		//-------------
		//左側
		//-------------
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-10.0f, -3.0f, 8.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 3.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);


		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-9.0f, -3.0f, 5.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 3.0f, 5.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 5.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-10.0f, -3.0f, 2.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 3.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-5.0f, -3.0f, 13.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(7.0f, 3.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(7.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-5.0f, -3.0f, 10.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 3.0f, 5.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 5.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-5.0f, -4.5f, -5.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 0.0f, 6.5f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 6.6f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		//-------------
		//右側
		//-------------
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(10.0f, -3.0f, 8.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(3.0f, 3.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(9.0f, -3.0f, 5.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
				XMFLOAT3(1.0f, 3.0f, 5.0f),	//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
				true,						//アクティブフラグ
				ColliderType::BOX			//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 5.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(10.0f, -3.0f, 2.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(3.0f, 3.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);


		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(5.0f, -3.0f, -3.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(7.0f, 3.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(7.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(5.0f, -3.0f, 0.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
				XMFLOAT3(1.0f, 3.0f, 5.0f),	//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
				true,						//アクティブフラグ
				ColliderType::BOX			//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 5.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(5.0f, -4.5f, 14.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 6.5f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 6.6f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		break;
	case 3: //ステージ3
		//-------------
		//左側
		//-------------
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-8.0f, -4.5f, -4.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 0.0f, 9.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 9.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-5.0f, -3.0f, 17.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 3.0f, 3.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 3.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		//-------------
		//真ん中
		//-------------

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(0.0f, -3.0f, 5.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	//回転
				XMFLOAT3(8.0f, 3.0f, 1.0f),	//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	//移動速度
				true,						//アクティブフラグ
				ColliderType::BOX,			//コライダータイプ
				XMFLOAT3(1.0f, 1.0f, 1.0f),	//コライダーボックスサイズ
				false,						//コライダーのトリガーフラグ
				1.0f,						//回転速度
				true,						//回転フラグ
				4.0f,						//回転半径
				XMFLOAT3(0.0f, 0.0f, 5.0f)	//回転中心座標
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(8.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		//-------------
		//右側
		//-------------
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(8.0f, -4.5f, 13.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 9.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 9.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);


		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(5.0f, -3.0f, -7.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 3.0f, 3.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 3.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		break;
	case 4: //ステージ4
		//-------------
		//左側
		//-------------
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-6.0f, -4.5f, 12.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 0.0f, 2.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 2.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-6.5f, -4.5f, 11.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(2.0f, 0.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-8.0f, -3.0f, 9.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 3.0f, 4.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 4.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-4.0f, -3.0f, 13.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 3.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-5.0f, -3.0f, -6.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 3.0f, 5.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 5.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		//-------------
		//真ん中
		//-------------
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-2.0f, -4.5f, 2.0f),//位置
				XMFLOAT3(0.0f, 45.0f, 0.0f), //回転
				XMFLOAT3(3.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX,			 //コライダータイプ
				XMFLOAT3(1.0f, 1.0f, 1.0f),	 //コライダーボックスサイズ
				false,						 //コライダーのトリガーフラグ
				1.0f,						 //回転速度	
				true,						 //回転フラグ
				3.5f,						 //回転半径
				XMFLOAT3(0.0f, -3.0f, 5.0f)  //回転中心座標
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(2.0f, -4.5f, 8.0f), //位置
				XMFLOAT3(0.0f, 90.0f, 0.0f), //回転
				XMFLOAT3(3.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX,			 //コライダータイプ
				XMFLOAT3(1.0f, 1.0f, 1.0f),	 //コライダーボックスサイズ
				false,						 //コライダーのトリガーフラグ
				1.0f,						 //回転速度	
				true,						 //回転フラグ
				3.5f,						 //回転半径
				XMFLOAT3(0.0f, -3.0f, 5.0f)  //回転中心座標
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-4.0f, -4.5f, 8.0f),//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f), //回転
				XMFLOAT3(3.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX,			 //コライダータイプ
				XMFLOAT3(1.0f, 1.0f, 1.0f),	 //コライダーボックスサイズ
				false,						 //コライダーのトリガーフラグ
				1.0f,						 //回転速度	
				true,						 //回転フラグ
				3.5f,						 //回転半径
				XMFLOAT3(0.0f, -3.0f, 5.0f)  //回転中心座標
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(4.0f, -4.5f, 2.0f), //位置
				XMFLOAT3(0.0f, 45.0f, 0.0f), //回転
				XMFLOAT3(3.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX,			 //コライダータイプ
				XMFLOAT3(1.0f, 1.0f, 1.0f),	 //コライダーボックスサイズ
				false,						 //コライダーのトリガーフラグ
				1.0f,						 //回転速度	
				true,						 //回転フラグ
				3.5f,						 //回転半径
				XMFLOAT3(0.0f, -3.0f, 5.0f)  //回転中心座標
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		//-------------
		//右側
		//-------------
		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(5.0f, -3.0f, 15.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 3.0f, 5.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 5.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(6.0f, -3.0f, -2.5f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 3.0f, 2.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 2.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(7.0f, -3.0f, -1.0f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 3.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 0.0f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(8.0f, -4.5f, 0.5f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 2.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 2.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(4.0f, -4.5f, -3.0f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 0.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(3.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		break;

	case 5:	//ステージ5

		//-------------
		//バネ
		//-------------

		m_pSprings.push_back(
			new Spring(
				MESH_TYPE::CUBE,
				XMFLOAT3(24.0f, -5.0f, 18.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.5f, 1.5f, 1.5f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				XMFLOAT3(0.0f, -4.0f, 5.0f),	  //発射ターゲット（このスプリング固有）
				true,					  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);

		m_pSprings.push_back(
			new Spring(
				MESH_TYPE::CUBE,
				XMFLOAT3(-24.0f, -5.0f, -7.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.5f, 1.5f, 1.5f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				XMFLOAT3(0.0f, -4.0f, 5.0f),	  //発射ターゲット（このスプリング固有）
				true,					  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);

		m_pSprings.push_back(
			new Spring(
				MESH_TYPE::CUBE,
				XMFLOAT3(24.0f, -5.0f, -7.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.5f, 1.5f, 1.5f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				XMFLOAT3(0.0f, -4.0f, 5.0f),	  //発射ターゲット（このスプリング固有）
				true,					  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);

		m_pSprings.push_back(
			new Spring(
				MESH_TYPE::CUBE,
				XMFLOAT3(-24.0f, -5.0f, 18.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.5f, 1.5f, 1.5f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				XMFLOAT3(0.0f, -4.0f, 5.0f),	  //発射ターゲット（このスプリング固有）
				true,					  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		//-------------
		//真ん中側
		//-------------
		m_pWallCurves.push_back(
			new WallCurve(
				MESH_TYPE::IMPORT,
				XMFLOAT3(0.0f, -4.5f, 8.0f),  //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 3.0f, 3.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX,			  //コライダータイプ
				XMFLOAT3(-1.5f, 1.5f, -2.5f), //コライダーボックスサイズ
				false,						  //コライダーのトリガーフラグ
				1.0f,						  //回転速度	
				true,						  //回転フラグ
				6.0f,						  //回転半径
				XMFLOAT3(0.0f, -4.5f, 5.0f),  //回転中心座標
				-180.0f
			)
		);
		m_pWallCurves.back()->SetDrawn(false);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-1.5f, 1.5f, -2.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -90.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-1.5f, 1.5f, -1.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -90.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-1.3f, 1.5f, -0.75f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -60.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-0.75f, 1.5f, -0.2f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -30.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(	//真ん中
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(	//真ん中
			ColliderType::BOX,
			XMFLOAT3(1.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(1.75f, 1.5f, -0.2f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 30.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(2.3f, 1.5f, -0.75f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 60.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(2.5f, 1.5f, -1.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 90.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(2.5f, 1.5f, -2.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 90.0f, 0.0f)
		);

		m_pWallCurves.push_back(
			new WallCurve(
				MESH_TYPE::IMPORT,
				XMFLOAT3(0.0f, -4.5f, 2.0f),  //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(3.0f, 3.0f, 3.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX,			  //コライダータイプ
				XMFLOAT3(-1.5f, 1.5f, -2.5f), //コライダーボックスサイズ
				false,						  //コライダーのトリガーフラグ
				1.0f,						  //回転速度	
				true,						  //回転フラグ
				6.0f,						  //回転半径
				XMFLOAT3(0.0f, -4.5f, 5.0f),  //回転中心座標
				-180.0f
			)
		);
		m_pWallCurves.back()->SetDrawn(false);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-1.5f, 1.5f, -2.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -90.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-1.5f, 1.5f, -1.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -90.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-1.3f, 1.5f, -0.75f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -60.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-0.75f, 1.5f, -0.2f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, -30.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(	//真ん中
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(	//真ん中
			ColliderType::BOX,
			XMFLOAT3(1.0f, 1.5f, 0.0f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(1.75f, 1.5f, -0.2f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 30.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(2.3f, 1.5f, -0.75f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 60.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(2.5f, 1.5f, -1.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 90.0f, 0.0f)
		);
		m_pWallCurves.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(2.5f, 1.5f, -2.5f),
			XMFLOAT3(1.1f, 3.1f, 1.1f),
			XMFLOAT3(0.0f, 90.0f, 0.0f)
		);
		///////////////////////////////
		//-------------
		//左側
		//-------------

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-7.5f, -5.0f, 12.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX					//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-0.5f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-10.0f, -5.0f, 12.0f),	//位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),	//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(-0.5f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-12.5f, -5.5f, 12.5f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-12.5f, -5.5f, 10.0f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-12.5f, -5.5f, 7.5f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-15.0f, -5.5f, 7.5f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.5f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(-17.5f, -5.5f, 7.5f), //位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.5f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-17.0f, -5.0f, 5.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-17.0f, -5.0f, 2.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-11.0f, -5.0f, -8.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-11.0f, -5.0f, -5.5f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(-11.0f, -5.0f, -3.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		/////////////////////////////////

		//-------------
		//右側
		//-------------

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(8.0f, -5.5f, -3.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(10.5f, -5.5f, -3.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(12.5f, -5.0f, 2.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(12.5f, -5.0f, -0.5f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(12.5f, -5.0f, -3.0f),	//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),		//スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),		//移動速度
				true,							//アクティブフラグ
				ColliderType::BOX				//コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(14.6f, -5.0f, 2.4f), //位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.4f, 1.0f, 0.4f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(17.0f, -5.0f, 2.4f), //位置
				XMFLOAT3(0.0f, 90.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.4f, 1.0f, 0.5f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(17.5f, -5.5f, 4.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWallPasses.push_back(
			new WallPass(
				MESH_TYPE::CUBE,
				XMFLOAT3(17.5f, -5.5f, 7.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //回転
				XMFLOAT3(1.0f, 0.0f, 1.0f),	 //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	 //移動速度
				true,						 //アクティブフラグ
				ColliderType::BOX			 //コライダータイプ
			)
		);
		m_pWallPasses.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.5f, 0.0f),
			XMFLOAT3(2.5f, 3.0f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(11.0f, -5.0f, 17.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.1f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);

		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(11.0f, -5.0f, 15.0f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.1f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);


		m_pWalls.push_back(
			new Wall(
				MESH_TYPE::CUBE,
				XMFLOAT3(11.0f, -5.0f, 12.5f),//位置
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //回転
				XMFLOAT3(1.0f, 1.0f, 1.0f),	  //スケール
				XMFLOAT3(0.0f, 0.0f, 0.0f),	  //移動速度
				true,						  //アクティブフラグ
				ColliderType::BOX			  //コライダータイプ
			)
		);
		m_pWalls.back()->GetColliderSet()->AddCollider(
			ColliderType::BOX,
			XMFLOAT3(0.0f, 1.0f, 0.0f),
			XMFLOAT3(2.5f, 3.1f, 2.5f),
			XMFLOAT3(0.0f, 0.0f, 0.0f)
		);
		break;

	}

	for (auto& wall : m_pWalls)
	{
		wall->GetColliderSet()->RegisterColliders(collisionManager);
	}

	for (auto& wallpass : m_pWallPasses)
	{
		wallpass->GetColliderSet()->RegisterColliders(collisionManager);
	}

	for (auto& wallcurve : m_pWallCurves)
	{
		wallcurve->GetColliderSet()->RegisterColliders(collisionManager);
	}

	for (auto& spring : m_pSprings)
	{
		spring->GetColliderSet()->RegisterColliders(collisionManager);
	}

	for (auto& ground : m_pGrounds)
	{
		ground->GetColliderSet()->RegisterColliders(collisionManager);
	}

}

//更新
void FieldManager::UpdateOverride()
{
	//壁更新	
	for (auto& i : m_pWalls)
	{
		if (i->IsActive())
		{
			i->Update();
		}
	}
	//弾貫通壁更新
	for (auto& i : m_pWallPasses)
	{
		if (i->IsActive())
		{
			i->Update();
		}
	}

	//カーブ壁更新
	for (auto& i : m_pWallCurves)
	{
		if (i->IsActive())
		{
			i->Update();
		}
	}

	//地面更新	
	for (auto& i : m_pGrounds)
	{
		if (i->IsActive())
		{
			i->Update();
		}
	}

	//バネ更新
	for (auto& i : m_pSprings)
	{
		if (i->IsActive())
		{
			i->Update();
		}
	}
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
			*wallpass,		//ゲームオブジェクト配列の参照
			m_wallPassInfo	//弾貫通壁描画情報
		);
	}

	//カーブ壁描画情報をシーンに提出
	for (auto& wallcurve : m_pWallCurves)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,		//シーンの参照
			*wallcurve,			//ゲームオブジェクト配列の参照
			m_wallCurveInfo		//壁描画情報
		);
	}

	//バネ描画情報をシーンに提出
	for (auto& spring : m_pSprings)
	{//描画要求をシーンに提出
		SubmitRenderInfo(
			renderer,			//シーンの参照
			*spring,			//ゲームオブジェクト配列の参照
			m_springInfo		//バネ描画情報
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
	m_pWallCurves.clear();
	m_pSprings.clear();
	m_pGrounds.clear();


	m_wallInfo.clear();
	m_wallPassInfo.clear();
	m_wallCurveInfo.clear();
	m_springInfo.clear();
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
		MESH_TYPE::IMPORT,
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		L"asset/fbx/wall_capacitor/ST_wall_capacitor.fbx",	//テクスチャのファイル名
		false,
		BILLBOARD_NONE,
		false,
		false
	);

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_wallPassInfo,				//描画情報構造体配列へのポインタ
		MESH_TYPE::IMPORT,
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		L"asset/fbx/hole/ST_hole.fbx",	//テクスチャのファイル名
		false,
		BILLBOARD_NONE,
		false,
		false
	);

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_wallCurveInfo,				//描画情報構造体配列へのポインタ
		m_pWallCurves[0]->GetMeshType(),//メッシュタイプ
		BLEND_MODE::BLEND_OPAQUE,		//ブレンドモード
		L"asset/fbx/wall_curve_ST.fbx",		//テクスチャのファイル名
		true,
		BILLBOARD_NONE,
		false,
		false
	);

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_springInfo,					//描画情報構造体配列へのポインタ
		MESH_TYPE::IMPORT,				//メッシュタイプ
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		springTexPath,
		true
	);
	//バネのアニメーションをバインド
	for(auto& spring : m_pSprings)
	{
		auto* set = spring->GetNodeAnimatorSet();          // Spring内部の実体
		auto* asset = m_springInfo[0].pNodeAnimAsset;

		set->pNodeAnimator->Bind(asset);
		set->isAnimLoaded = true;
		set->isAnimPlaying = true;
	}

	CreateRenderInfo(
		textureManager,					//テクスチャマネージャへの参照
		meshManager,					//メッシュマネージャへの参照
		&m_groundInfo,					//描画情報構造体配列へのポインタ
		MESH_TYPE::IMPORT,
		BLEND_MODE::BLEND_MASKED,		//ブレンドモード
		L"asset/fbx/stage/ST_stage.fbx",	//テクスチャのファイル名
		false,
		BILLBOARD_NONE,
		false,
		false
	);
}
