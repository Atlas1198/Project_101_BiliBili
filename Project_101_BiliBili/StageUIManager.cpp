#include "StageUIManager.h"


using namespace DirectX;

//デストラクタ
StageUIManager::~StageUIManager()
{
}

//初期化
void StageUIManager::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	//パス
	const std::wstring BACKGROUND_PATH[STAGE_NUM] = 
	{
 		L"asset/texture/stage_scene/UI_STAGE_Back_G.png",
		L"asset/texture/stage_scene/UI_STAGE_Back_R.png",
		L"asset/texture/stage_scene/UI_STAGE_Back_B.png"
	};
	const std::wstring EXPLANATION_PATH[STAGE_NUM] =
	{
		L"asset/texture/stage_scene/UI_STAGE_Explanation_G.png",
		L"asset/texture/stage_scene/UI_STAGE_Explanation_R.png",
		L"asset/texture/stage_scene/UI_STAGE_Explanation_B.png",
	};
	const std::wstring NAME_PATH[STAGE_NUM] =
	{
		L"asset/texture/stage_scene/UI_STAGE_Name_G.png",
		L"asset/texture/stage_scene/UI_STAGE_Name_R.png",
		L"asset/texture/stage_scene/UI_STAGE_Name_B.png"
	};
	//サイズ
	const XMFLOAT3 BACKGROUND_SIZE{ m_screenWidth, m_screenHeight, 0.0f };
	const XMFLOAT3 EXPLANATION_SIZE{ 751.0f, 169.0f, 0.0f };
	const XMFLOAT3 NAME_SIZE{ 735.0f, 229.0f, 0.0f };

	//座標
	const XMFLOAT3 BACKGROUND_POSITION{ 0.0f, 0.0f, 0.0f };
	const XMFLOAT3 EXPLANATION_POSITION{ m_screenWidth * 0.5f + EXPLANATION_SIZE.x * 0.5f, 0.0f, 0.0f };
	const XMFLOAT3 NAME_POSITION{ 450.0f, -300.0f, 0.0f };

	//説明文の目的地X座標を計算
	m_explanationDestinationX = m_screenWidth * 0.5f - EXPLANATION_SIZE.x * 0.5f;
	m_explanationOriginalPosition = EXPLANATION_POSITION;

	for (int i = 0; i < STAGE_NUM; i++)
	{
		m_pBackGround[i] = new UIImage(
			XMFLOAT3(
				BACKGROUND_POSITION.x,
				BACKGROUND_POSITION.y, 
				BACKGROUND_POSITION.z
			),
			BACKGROUND_SIZE,
			{ 0.0f, 0.0f, 0.0f },
			0,
			BACKGROUND_PATH[i].c_str(),
			PSO_KEY_MASKED
		);
		m_pExplanation[i] = new UIImage(
			EXPLANATION_POSITION,
			EXPLANATION_SIZE,
			{ 0.0f, 0.0f, 0.0f },
			2,
			EXPLANATION_PATH[i].c_str(),
			PSO_KEY_TRANSPARENT
		);
		m_pName[i] = new UIImage(
			NAME_POSITION,
			NAME_SIZE,
			{ 0.0f, 0.0f, 0.0f },
			3,
			NAME_PATH[i].c_str(),
			PSO_KEY_MASKED
		);

		m_roots.push_back(std::unique_ptr<UIImage>(m_pBackGround[i]));
		m_roots.push_back(std::unique_ptr<UIImage>(m_pExplanation[i]));
		m_roots.push_back(std::unique_ptr<UIImage>(m_pName[i]));

		m_pBackGround[i]->SetActive(false);
		m_pExplanation[i]->SetActive(false);
		m_pName[i]->SetActive(false);
	}

	m_pBackGround[0]->SetActive(true);
	m_pExplanation[0]->SetActive(true);
	m_pName[0]->SetActive(true);


	m_pHeader = new UIImage(
		XMFLOAT3(0.0f, m_screenHeight * 0.5f - 76.5f, 0.0f),
		XMFLOAT3(m_screenWidth, 153.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		100,
		L"asset/texture/stage_scene/UI_STAGE_Header.png",
		PSO_KEY_MASKED
	);

	m_pOperation = new UIImage(
		XMFLOAT3(0.0f, -m_screenHeight * 0.5f + 38.5f, 0.0f),
		XMFLOAT3(m_screenWidth, 77.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		100,
		L"asset/texture/stage_scene/UI_STAGE_Operation.png",
		PSO_KEY_MASKED
	);

	m_pArrow = new UIImage(
		NAME_POSITION,
		XMFLOAT3(937.0f, 124.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		100,
		L"asset/texture/stage_scene/UI_STAGE_Select.png",
		PSO_KEY_TRANSPARENT
	);
	const XMFLOAT2 NAME_BACK_POSITION_OFFSET = { 4.0f, -6.0f };
	m_pNameBack = new UIImage(
		XMFLOAT3(
			NAME_POSITION.x + NAME_BACK_POSITION_OFFSET.x,
			NAME_POSITION.y + NAME_BACK_POSITION_OFFSET.y,
			NAME_POSITION.z
			),
		XMFLOAT3(743.0f, 242.0f, 0.0f),
		XMFLOAT3(0.0f, 0.0f, 0.0f),
		1,
		L"asset/texture/stage_scene/UI_STAGE_Name_EF.png",
		PSO_KEY_TRANSPARENT
	);
	m_nameBackOriginalScale = m_pNameBack->GetLocalScale();

	m_roots.push_back(std::unique_ptr<UIImage>(m_pHeader));
	m_roots.push_back(std::unique_ptr<UIImage>(m_pOperation));
	m_roots.push_back(std::unique_ptr<UIImage>(m_pArrow));
	m_roots.push_back(std::unique_ptr<UIImage>(m_pNameBack));

	m_isSelectAnimationStarted = false;
	m_currentStageType = STAGE_TYPE::STAGE_GREEN;
}

//更新
void StageUIManager::UpdateOverride()
{
	m_timer++;	//タイマー更新

	//ステージ名背景アニメーション
	if (m_isSelectAnimationStarted)
	{//選択アニメーション開始後
		m_selectAnimationTimer++;
		const float changeAmount = 1.04f;
		if (m_selectAnimationTimer <= 3)
		{//拡大
			auto scale = m_pNameBack->GetLocalScale();
			scale.x *= changeAmount;
			scale.y *= changeAmount;
			m_pNameBack->SetLocalScale(scale);
		}
		else if (m_selectAnimationTimer <= 6)
		{//縮小
			auto scale = m_pNameBack->GetLocalScale();
			scale.x /= changeAmount;
			scale.y /= changeAmount;
			m_pNameBack->SetLocalScale(scale);
		}
	}
	else
	{//選択アニメーション開始前
		auto nameBackScale = m_pNameBack->GetLocalScale();
		float scaleFactor = 10.0f * (sinf(m_timer * 0.05f) + 1.0f);
		nameBackScale.x = m_nameBackOriginalScale.x + scaleFactor;
		nameBackScale.y = m_nameBackOriginalScale.y + scaleFactor;
		m_pNameBack->SetLocalScale(nameBackScale);
	}

	//説明文アニメーション
	for(int i = 0; i < STAGE_NUM; i++)
	{
		if(m_pExplanation[i]->IsActive())
		{
			auto explanationPos = m_pExplanation[i]->GetLocalPosition();
			explanationPos.x -= 60.0f;
			if(explanationPos.x < m_explanationDestinationX)
			{
				explanationPos.x = m_explanationDestinationX;
			}
			m_pExplanation[i]->SetLocalPosition(explanationPos);
		}
	}

	//矢印アニメーション
	if(m_arrowDirection != DIRECTION::NONE)
	{
		const float MOVE_DURATION = 5.0f;
		const float DIRECTION_SWITCH_TIME = MOVE_DURATION * 0.5f;
		const int DIRECTION = (m_arrowDirection == DIRECTION::LEFT) ? -1 : 1;

		if(m_arrowMoveTimer <= DIRECTION_SWITCH_TIME)
		{
			auto arrowPos = m_pArrow->GetLocalPosition();
			arrowPos.x += DIRECTION * 5.0f;	//矢印を移動
			m_pArrow->SetLocalPosition(arrowPos);
		}
		else if(m_arrowMoveTimer <= MOVE_DURATION)
		{
			auto arrowPos = m_pArrow->GetLocalPosition();
			arrowPos.x -= DIRECTION * 5.0f;	//矢印を元の位置に戻す
			m_pArrow->SetLocalPosition(arrowPos);
		}

		//移動終了後に方向をNONEにする
		if(m_arrowMoveTimer > MOVE_DURATION)
		{
			m_pArrow->SetLocalPosition({ 450.0f, -300.0f, 0.0f });	//矢印の位置をリセット
			m_arrowDirection = DIRECTION::NONE;
		}

		m_arrowMoveTimer++;	//矢印移動タイマー更新
	}

}

//終了
void StageUIManager::FinalizeOverride()
{
}

//ステージUI変更関数
void StageUIManager::ChangeStageUI(STAGE_TYPE nextStageType)
{
	for(int i = 0; i < STAGE_NUM; i++)
	{
		m_pBackGround[i]->SetActive(false);
		m_pExplanation[i]->SetActive(false);
		m_pName[i]->SetActive(false);
	}

	int index = static_cast<int>(nextStageType);
	m_pBackGround[index]->SetActive(true);
	m_pExplanation[index]->SetActive(true);
	m_pName[index]->SetActive(true);

	//説明文の位置リセット
	m_pExplanation[index]->SetLocalPosition(m_explanationOriginalPosition);

	//矢印の移動方向を設定
	int currentIndex = static_cast<int>(m_currentStageType);
	int nextIndex = static_cast<int>(nextStageType);
	int moveDir = (currentIndex - nextIndex + STAGE_NUM) % STAGE_NUM;
	if(moveDir == 1)
	{
		m_arrowDirection = DIRECTION::LEFT;
	}
	else if(moveDir == STAGE_NUM - 1)
	{
		m_arrowDirection = DIRECTION::RIGHT;
	}
	m_pArrow->SetLocalPosition({ 450.0f, -300.0f, 0.0f });	//矢印の位置をリセット
	m_arrowMoveTimer = 0;	//矢印移動タイマーリセット
	m_currentStageType = nextStageType;
}

//ステージ選択アニメーション開始関数
void StageUIManager::StartSelectAnimation()
{
	m_isSelectAnimationStarted = true;
	m_selectAnimationTimer = 0;
}
