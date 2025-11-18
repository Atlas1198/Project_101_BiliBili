#pragma once
#include "Camera.h"
#include "TextureManager.h"
#include "ComPtr.h"
#include "SharedStruct.h"
#include "CollisionManager.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

//シーンクラス基底
//全シーンクラスの基底クラス
class SceneBase
{
public:	//公開関数
	SceneBase(float window_width, float window_height);	//コンストラクタ
	~SceneBase() {};									//デストラクタ

	//メイン処理関数	
	void Initialize(	//初期化
		InputManager* pInputManager, 
		TextureManager& pTextureManager,
		MeshManager& pMeshManager
	);
	virtual void InitializeOverride(	//シーン固有の初期化
		InputManager* pInputManager,
		TextureManager& pTextureManager,
		MeshManager& pMeshManager
	) = 0;
	void Update();										//更新
	virtual void UpdateOverride() = 0;					//シーン固有の更新
	void Draw(Renderer& pRenderer);						//描画
	virtual void DrawOverride(Renderer& pRenderer) = 0;	//シーン固有の描画
	void Finalize();									//終了	
	virtual void FinalizeOverride() {};					//シーン固有の終了
	virtual void ResolveCollisions() = 0;				//衝突後処理

	//ゲッター
	CameraInfo* GetCameraInfo() const;			//カメラ情報取得

protected:
	Camera* m_pCamera = nullptr;						//カメラ
	InputManager* m_pInputManager = nullptr;			//入力管理クラス
	CollisionManager* m_pCollisionManager = nullptr;	//衝突管理クラス
};