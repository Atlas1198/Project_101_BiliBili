#include "SceneBase.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

using namespace RenderData;

//コンストラクタ
SceneBase::SceneBase(float window_width, float window_height)
{
	m_pCamera = new Camera(window_width, window_height);	//カメラ生成
	m_pCollisionManager = new CollisionManager();			//衝突管理クラス生成


}

//初期化
void SceneBase::Initialize(
	InputManager* pInputManager,
	TextureManager& pTextureManager,
	MeshManager& pMeshManager
)
{
	//入力管理クラス保存
	m_pInputManager = pInputManager;

	//カメラ初期化
	m_pCamera->Initialize(pInputManager);

	//初期化
	m_pCollisionManager->Initialize(pTextureManager, pMeshManager);

	//シーン固有の初期化呼び出し
	InitializeOverride(pInputManager, pTextureManager, pMeshManager);

}

//更新
void SceneBase::Update()
{
	//シーン固有の更新呼び出し
	UpdateOverride();

	//カメラ更新
	m_pCamera->Update();

	//衝突管理クラス更新
	//m_pCollisionManager->CheckCollisions();

	//衝突後処理
	//ResolveCollisions();
}

//描画
void SceneBase::Draw(Renderer& pRenderer)
{
	//シーン固有の描画呼び出し
	DrawOverride(pRenderer);

#ifdef _DEBUG
	//衝突管理クラス描画
	m_pCollisionManager->Draw(pRenderer);
#endif // _DEBUG
}

//カメラ情報取得
CameraInfo* SceneBase::GetCameraInfo() const
{
	return m_pCamera->GetCameraInfo();
}