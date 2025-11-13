#include "TitleScene.h"
#include "Renderer.h"
#include "InputManager.h"
#include "TextureManager.h"
#include "MeshManager.h"
using namespace RenderData;

//コンストラクタ
TitleScene::TitleScene(float window_width, float window_height)
	: SceneBase(window_width, window_height)
{
}

//デストラクタ
TitleScene::~TitleScene()
{
}

//初期化
void TitleScene::InitializeOverride(
	InputManager* pInputManager,		//入力管理クラスのポインタ
	TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
	MeshManager& pMeshManager			//メッシュ管理クラスの参照
)
{
}

//更新
void TitleScene::UpdateOverride()
{
}

//描画
void TitleScene::DrawOverride(Renderer& pRenderer)
{
}

//終了
void TitleScene::Finalize()
{
	
}

//カメラ情報取得
CameraInfo* TitleScene::GetCameraInfo() const
{
	return m_pCamera->GetCameraInfo();
}
