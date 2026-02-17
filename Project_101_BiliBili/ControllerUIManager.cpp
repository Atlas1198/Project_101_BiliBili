#include "ControllerUIManager.h"
#include "EventManager.h"
#include "InputManager.h"

using namespace DirectX;

//コントローラーUI管理クラス初期化
void ControllerUIManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	m_pBackgroundImage = new UIImage(
		{ 0.0f, 0.0f, 0.0f },	//位置
		{ m_screenWidth, m_screenHeight, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		0,						//描画順序
		L"asset/texture/white.png",
		PSO_KEY_OPAQUE
	);
	m_pBackgroundImage->SetColor({ 0.9f, 0.9f, 0.9f, 1.0f });
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBackgroundImage));

	m_pHeaderImage = new UIImage(
		{ 0.0f, m_screenHeight * 0.5f - 144.5f, 0.0f },	//位置
		{ m_screenWidth, 289.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		1,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Header.png",
		PSO_KEY_MASKED
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pHeaderImage));

	m_pControllerIconBase = new UIImage(
		{ 0.0f, 0.0f, 0.0f },	//位置
		{ 1777, 414.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		2,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con.png",
		PSO_KEY_MASKED
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pControllerIconBase));

	float edge = 1777.0f * 0.5f;
	float posEdge = edge - 415.0f * 0.5f;

	m_pControllerIconUIs[0] = new ControllerIconUI(
		{ -posEdge, 0.0f, 0.0f },	//位置
		{ 1.0f, 1.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_1.png",
		PSO_KEY_TRANSPARENT
	);

	m_pControllerIconUIs[1] = new ControllerIconUI(
		{ -posEdge + 39 + 415.0f, 0.0f, 0.0f },	//位置
		{ 1.0f, 1.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_2.png",
		PSO_KEY_TRANSPARENT
	);

	m_pControllerIconUIs[2] = new ControllerIconUI(
		{ 39 * 0.5f + 415.0f * 0.5f, 0.0f, 0.0f },	//位置
		{ 1.0f, 1.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_3.png",
		PSO_KEY_TRANSPARENT
	);

	m_pControllerIconUIs[3] = new ControllerIconUI(
		{ posEdge, 0.0f, 0.0f },	//位置
		{ 1.0f, 1.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },		//回転
		3,							//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_4.png",
		PSO_KEY_TRANSPARENT
	);

	for(auto& icon : m_pControllerIconUIs)
	{
		icon->SetActive(false);
		m_roots.push_back(std::unique_ptr<UIBase>(icon));
	}

	m_pGoToNextSceneIcon = new UIImage(
		{ 0.0f, -370.0f, 0.0f },	//位置
		{ 878.0f, 116.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		4,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Go.png",
		PSO_KEY_TRANSPARENT
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pGoToNextSceneIcon));

	//テスト用エフェクトコマンド追加
	using args = int;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::CONTROLLER_CONNECTED,
		[this](std::shared_ptr<args> data)
		{
			SetIconConnected(*data);
		}
	);

	using args2 = std::pair<int, InputInfo&>;
	EventManager::GetInstance()->Subscribe<args2>(
		EventType::CONTROLLER_ICON_REACTION,
		[this](std::shared_ptr<args2> data)
		{
			ConnectedIconReaction(data->first, data->second);
		}
	);

	m_passedFrameCount = 0;
}

//コントローラーUI管理クラス更新
void ControllerUIManager::UpdateOverride()
{
	m_passedFrameCount++;

	//次のシーンへ進むアイコンのスケールアニメーション
	float scaleFactor = 1.0f + 0.05f * std::sin(static_cast<float>(m_passedFrameCount) * 0.04f);
	m_pGoToNextSceneIcon->SetLocalScale( { 878.0f * scaleFactor, 116.0f * scaleFactor, 1.0f } );
}

//コントローラーUI管理クラス終了
void ControllerUIManager::FinalizeOverride()
{
}

//コントローラーアイコンを接続済みに設定
void ControllerUIManager::SetIconConnected(int index)
{
	if (index < 0 || index >= 4)
	{
		return;
	}

	m_pControllerIconUIs[index]->SetActive(true);
}

//コントローラー接続アイコンの入力リアクション
void ControllerUIManager::ConnectedIconReaction(int index, const InputInfo& inputInfo)
{
	if (index < 0 || index >= 4)
	{
		return;
	}
	m_pControllerIconUIs[index]->ActivateReaction(inputInfo);
}
