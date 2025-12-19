#include "SelectionScene.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"

//コンストラクタ
SelectionScene::SelectionScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
}

//デストラクタ
SelectionScene::~SelectionScene()
{
}

//初期化
void SelectionScene::InitializeOverride(
	InputManager* pInputManager,		//入力管理クラスのポインタ
	TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
	MeshManager& pMeshManager			//メッシュ管理クラスの参照
)
{
}

//更新
void SelectionScene::UpdateOverride()
{
}

//描画
void SelectionScene::DrawOverride(Renderer& pRenderer)
{
}

//終了
void SelectionScene::FinalizeOverride()
{
}

//カメラ情報取得
CameraInfo* SelectionScene::GetCameraInfo() const
{
	return m_pCamera->GetCameraInfo();
}
