#include "ControllerUIManager.h"
#include "EventManager.h"
#include "InputManager.h"

//コントローラーUI管理クラス初期化
void ControllerUIManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	//テスト用エフェクトコマンド追加
	using args = int;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::CONTROLLER_CONNECTED,
		[this](std::shared_ptr<args> data)
		{
			SetIconConnected(*data);
		}
	);

	using args = std::pair<int, InputInfo&>;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::CONTROLLER_ICON_REACTION,
		[this](std::shared_ptr<args> data)
		{
			ConnectedIconReaction(data->first, data->second);
		}
	);

}

//コントローラーUI管理クラス更新
void ControllerUIManager::UpdateOverride()
{
}

//コントローラーUI管理クラス終了
void ControllerUIManager::FinalizeOverride()
{
}

//描画情報準備
void ControllerUIManager::PrepareRenderInfo(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
}

//コントローラーアイコンを接続済みに設定
void ControllerUIManager::SetIconConnected(int index)
{
}

//コントローラー接続アイコンの入力リアクション
void ControllerUIManager::ConnectedIconReaction(int index, const InputInfo& inputInfo)
{
}
