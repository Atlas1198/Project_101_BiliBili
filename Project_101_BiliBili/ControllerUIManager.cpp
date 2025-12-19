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
		BLEND_MODE::BLEND_OPAQUE
	);
	m_pBackgroundImage->SetColor({ 0.9f, 0.9f, 0.9f, 1.0f });
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBackgroundImage));

	m_pHeaderImage = new UIImage(
		{ 0.0f, m_screenHeight * 0.5f - 144.5f, 0.0f },	//位置
		{ m_screenWidth, 289.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		1,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Header.png",
		BLEND_MODE::BLEND_MASKED
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pHeaderImage));

	m_pControllerIconBase = new UIImage(
		{ 0.0f, 0.0f, 0.0f },	//位置
		{ 1777, 414.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		2,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con.png",
		BLEND_MODE::BLEND_MASKED
	);
	m_roots.push_back(std::unique_ptr<UIBase>(m_pControllerIconBase));

	float edge = 1777.0f * 0.5f;
	float posEdge = edge - 415.0f * 0.5f;
	const float scaleFactor = 1.4f;

	m_pControllerIcons[0] = new UIImage(
		{ -posEdge, 0.0f, 0.0f },	//位置
		{ 415.0f * scaleFactor, 414.0f * scaleFactor, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_1.png",
		BLEND_MODE::BLEND_TRANSPARENT
	);

	m_pControllerIcons[1] = new UIImage(
		{ -posEdge + 39 + 415.0f, 0.0f, 0.0f },	//位置
		{ 415.0f * scaleFactor, 414.0f * scaleFactor, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_2.png",
		BLEND_MODE::BLEND_TRANSPARENT
	);

	m_pControllerIcons[2] = new UIImage(
		{ 39 * 0.5f + 415.0f * 0.5f, 0.0f, 0.0f },	//位置
		{ 415.0f * scaleFactor, 414.0f * scaleFactor, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_3.png",
		BLEND_MODE::BLEND_TRANSPARENT
	);

	m_pControllerIcons[3] = new UIImage(
		{ posEdge, 0.0f, 0.0f },	//位置
		{ 415.0f * scaleFactor, 414.0f * scaleFactor, 1.0f },	//スケール
		{ 0.0f, 0.0f, 0.0f },		//回転
		3,							//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Con_4.png",
		BLEND_MODE::BLEND_TRANSPARENT
	);

	for(auto& icon : m_pControllerIcons)
	{
		icon->SetActive(false);
		m_roots.push_back(std::unique_ptr<UIBase>(icon));
	}

	m_pGoToNextSceneIcon = new UIImage(
		{ 0.0f, -350.0f, 0.0f },	//位置
		{ 878.0f, 116.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		4,						//描画順序
		L"asset/texture/controller_scene/UI_CONTROLLER_Go.png",
		BLEND_MODE::BLEND_TRANSPARENT
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

	//接続済みコントローラーアイコンのスケール調整
	for(auto& controller : m_pControllerIcons)
	{
		if(controller->IsActive())
		{
			auto scale = controller->GetLocalTransform().scale;
			scale.x *= 0.95f;
			scale.y *= 0.95f;

			XMFLOAT3 setScale =
			{
				(std::max)(415.0f, scale.x),
				(std::max)(414.0f, scale.y),
				1.0f
			};

			controller->SetLocalTransform(
				{
					controller->GetLocalTransform().position,
					setScale,
					controller->GetLocalTransform().rotation
				}
			);
		}
	}

	float scaleFactor = 1.0f + 0.05f * std::sin(static_cast<float>(m_passedFrameCount) * 0.04f);
	m_pGoToNextSceneIcon->SetLocalTransform(
		{
			m_pGoToNextSceneIcon->GetLocalTransform().position,
			{ 878.0f * scaleFactor, 116.0f * scaleFactor, 1.0f },
			m_pGoToNextSceneIcon->GetLocalTransform().rotation
		}
	);
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
	m_pBackgroundImage->PrepareRenderInfo(textureManager, meshManager);
	m_pHeaderImage->PrepareRenderInfo(textureManager, meshManager);
	m_pControllerIconBase->PrepareRenderInfo(textureManager, meshManager);
	for (auto& icon : m_pControllerIcons)
	{
		icon->PrepareRenderInfo(textureManager, meshManager);
	}
	m_pGoToNextSceneIcon->PrepareRenderInfo(textureManager, meshManager);
}

//コントローラーアイコンを接続済みに設定
void ControllerUIManager::SetIconConnected(int index)
{
	if (index < 0 || index >= 4)
	{
		return;
	}
	m_pControllerIcons[index]->SetActive(true);
}

//コントローラー接続アイコンの入力リアクション
void ControllerUIManager::ConnectedIconReaction(int index, const InputInfo& inputInfo)
{

}
