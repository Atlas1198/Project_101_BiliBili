#include "TitleUI.h"

using namespace DirectX;

//コンストラクタ
TitleUI::TitleUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
}

//初期化
void TitleUI::InitializeOverride(
	TextureManager& textureManager, 
	MeshManager& meshManager
)
{
}

//更新
void TitleUI::UpdateOverride()
{
}

//終了
void TitleUI::FinalizeOverride()
{
}


void TitleUI::PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
	TextureManager& textureManager,	//テクスチャ管理クラスの参照
	MeshManager& meshManager		//メッシュ管理クラスの参照
)
{
}