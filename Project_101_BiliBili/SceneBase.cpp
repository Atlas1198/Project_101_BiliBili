#include "SceneBase.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"



//コンストラクタ
SceneBase::SceneBase(float window_width, float window_height)
{
	m_pCamera = new Camera(window_width, window_height);	//カメラ生成
	m_pCollisionManager = new CollisionManager();			//衝突管理クラス生成
	m_pEffectManager = new EffectManager();				//エフェクト管理クラス生成
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

	//エフェクト管理クラス初期化
	m_pEffectManager->Initialize(pTextureManager, pMeshManager);

	//シーン固有の初期化呼び出し
	InitializeOverride(pInputManager, pTextureManager, pMeshManager);

}

//更新
void SceneBase::Update()
{
	if(m_pInputManager->GetInputInfo()->key.p.trigger)
	{
		m_drawColliders = !m_drawColliders;
	}

	//シーン固有の更新呼び出し
	UpdateOverride();

	//カメラ更新
	m_pCamera->Update();

	//コライダー登録確認
	m_pCollisionManager->CheckColliders();

	//衝突管理クラス更新
	m_pCollisionManager->CheckCollisions();

	//衝突後処理
	ResolveCollisions();

	//エフェクト管理クラス更新
	m_pEffectManager->Update();
}

//描画
void SceneBase::Draw(Renderer& pRenderer)
{
	//シーン固有の描画呼び出し
	DrawOverride(pRenderer);

	m_pEffectManager->SubmitDraws(pRenderer);

#ifdef _DEBUG
	if (m_drawColliders)
	{
		//衝突管理クラス描画
		m_pCollisionManager->Draw(pRenderer);
	}
#endif // _DEBUG
}

//終了
void SceneBase::Finalize()
{
	//シーン固有の終了呼び出し
	FinalizeOverride();
	m_pCollisionManager->ClearColliders();
}

//カメラ情報取得
CameraInfo* SceneBase::GetCameraInfo() const
{
	return m_pCamera->GetCameraInfo();
}