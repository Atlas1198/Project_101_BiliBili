#include "CharacterUIManager.h"
#include "EventManager.h"

//デストラクタ
CharacterUIManager::~CharacterUIManager()
{
}

//初期化
void CharacterUIManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
}

//更新
void CharacterUIManager::UpdateOverride()
{
}

//終了
void CharacterUIManager::FinalizeOverride()
{
}

//オブジェクトの描画情報生成
void CharacterUIManager::PrepareRenderInfo(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
}
