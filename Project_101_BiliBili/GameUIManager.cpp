#include "GameUIManager.h"
#include "EventManager.h"

//デストラクタ
GameUIManager::~GameUIManager()
{
}

//初期化
void GameUIManager::InitializeOverride(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	m_pBBEffect = new BBSceneEffectUI(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		-1										//描画順序
	);
	m_pBBEffect->SetActive(false);

	//チームUIの初期化
	{
		m_pTeamUI1 = (new TeamUI(
			DirectX::XMFLOAT3{ -530.0f, 400.0f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },				//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
			0,													//描画順序
			L"asset/texture/game_scene/UI_INGAME_L_frame.png",	//フレームテクスチャパス
			L"asset/texture/game_scene/UI_INGAME_L_gauge.png",	//ゲージテクスチャパス
			L"asset/texture/game_scene/UI_INGAME_L_back.png",	//背景テクスチャパス
			L"asset/texture/game_scene/UI_INGAME_L_alert.png",	//アラートテクスチャパス
			L"asset/texture/game_scene/UI_INGAME_L_icon.png",	//アイコンテクスチャパス
			DIRECTION::LEFT										//オフセット方向
		));

		m_pTeamUI2 = (new TeamUI(
			DirectX::XMFLOAT3{ 530.0f, 400.0f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },				//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
			0,													//描画順序
			L"asset/texture/game_scene/UI_INGAME_R_frame.png",	//フレームテクスチャパス
			L"asset/texture/game_scene/UI_INGAME_R_gauge.png",	//ゲージテクスチャパス
			L"asset/texture/game_scene/UI_INGAME_R_back.png",	//背景テクスチャパス
			L"asset/texture/game_scene/UI_INGAME_R_alert.png",	//アラートテクスチャパス
			L"asset/texture/game_scene/UI_INGAME_R_icon.png",	//アイコンテクスチャパス
			DIRECTION::RIGHT									//オフセット方向
		));
	}

	//弾数UIの初期化
	{
		for (auto& bulletUI : m_pBulletCountUI1)
		{
			bulletUI = (new BulletCountUI(
				DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
				DirectX::XMFLOAT3{ 0.9f, 0.9f, 1.0f },	//スケール
				DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
				-1,											//描画順序
				L"asset/texture/game_scene/UI_INGAME_Rest_B.png",
				L"asset/texture/game_scene/UI_INGAME_NoRest_B.png"
			));
			bulletUI->SetActive(false); // 初期状態では非表示にする
		}

		for (auto& bulletUI : m_pBulletCountUI2)
		{
			bulletUI = (new BulletCountUI(
				DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
				DirectX::XMFLOAT3{ 0.9f, 0.9f, 1.0f },	//スケール
				DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
				-1,											//描画順序
				L"asset/texture/game_scene/UI_INGAME_Rest_R.png",
				L"asset/texture/game_scene/UI_INGAME_NoRest_R.png"
			));
			bulletUI->SetActive(false); // 初期状態では非表示にする
		}
	}

	{//プレイヤーポインターUIの初期化
		m_pPlayerPointerUI = (new PlayerPointerUI(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
			-1										//描画順序
		));
	}

	//カットインUIの初期化
	{
		m_pCutInUI1 = (new CutInUI(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
			0,										//描画順序
			DIRECTION::LEFT							//カットインの方向
		));

		m_pCutInUI2 = (new CutInUI(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
			0,										//描画順序
			DIRECTION::RIGHT						//カットインの方向
		));
	}

	//操作ガイド画像UIの初期化
	{
		const float sizeFactor = 0.75f; // サイズ調整用の係数
		m_pOperationGuideImage = (new UIImage(
			DirectX::XMFLOAT3{ 0.0f, 450.0f, 0.0f },				//位置
			DirectX::XMFLOAT3{ 276.0f * sizeFactor, 128.0f * sizeFactor, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },					//回転
			0,														//描画順序
			L"asset/texture/game_scene/UI_INGAME_Operation.png",	//テクスチャパス
			PSO_KEY_TRANSPARENT										//ブレンドモード
		));
	}

	//カウントUIの初期化
	{
		m_pCountUI = (new CountUI(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
			0										//描画順序
		));
	}

	//イベントアナウンスUIの初期化
	{
		m_pEventAnounceUI = (new EventAnounceUI(
			DirectX::XMFLOAT3{ 0.0f, 200.0f, 0.0f },	//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
			0										//描画順序
		));
	}

	//ゴールアナウンス画像UIの初期化
	{
		m_pGoalAnounceImage = (new UIImage(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 1297.0f, 432.0f, 1.0f },		//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
			0,												//描画順序
			L"asset/texture/game_scene/UI_INGAME_Goal.png",	//テクスチャパス
			PSO_KEY_TRANSPARENT								//ブレンドモード
		));
		m_pGoalAnounceImage->SetColor({ 1.0f, 1.0f, 1.0f, 0.0f }); // 初期状態ではアルファ０
	}

	//リザルトUIの初期化
	{
		m_pResultUI = (new ResultUI(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
			DirectX::XMFLOAT3{ 1.0f, 1.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
			0										//描画順序
		));
		m_pResultUI->SetActive(false); // 初期状態では非表示にする
	}

	//操作ガイド画像UIの初期化
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBBEffect));			//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pTeamUI1));				//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pTeamUI2));				//ルートUIオブジェクト配列に追加
	for (auto& bulletUI : m_pBulletCountUI1)
	{
		m_roots.push_back(std::unique_ptr<UIBase>(bulletUI));			//ルートUIオブジェクト配列に追加
	}
	for (auto& bulletUI : m_pBulletCountUI2)
	{
		m_roots.push_back(std::unique_ptr<UIBase>(bulletUI));			//ルートUIオブジェクト配列に追加
	}
	m_roots.push_back(std::unique_ptr<UIBase>(m_pPlayerPointerUI));		//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pCutInUI1));			//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pCutInUI2));			//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pOperationGuideImage));	//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pCountUI));				//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pResultUI));			//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pEventAnounceUI));		//ルートUIオブジェクト配列に追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pGoalAnounceImage));	//ルートUIオブジェクト配列に追加

	EventManager::GetInstance()->Subscribe<std::pair<int, float>>(
		EventType::UPDATE_HP_UI,
		[this](std::shared_ptr<std::pair<int, float>> data)
		{
			int teamID = data->first;
			float newHP = data->second;
			OnHPChanged(teamID, newHP);
		}
	);

	using args = std::pair<int, int>;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::UPDATE_BULLET_UI,
		[this](std::shared_ptr<args> data)
		{
			int teamID = data->first;
			int newCount = data->second;
			OnBulletCountChanged(teamID, newCount);
		}
	);

	EventManager::GetInstance()->Subscribe<args>(
		EventType::BB_CUT_IN,
		[this](std::shared_ptr<args> data)
		{
			CallCutIn(data->first, data->second);
		}
	);

	EventManager::GetInstance()->Subscribe<int>(
		EventType::SHOW_COUNT_UI,
		[this](std::shared_ptr<int> data)
		{
			ShowCountUI(*data);
		}
	);

	EventManager::GetInstance()->Subscribe<void>(
		EventType::SHOW_START_UI,
		[this](std::shared_ptr<void> data)
		{
			ShowStartUI();
		}
	);

	EventManager::GetInstance()->Subscribe<void>(
		EventType::SHOW_FINISH_UI,
		[this](std::shared_ptr<void> data)
		{
			ShowFinishUI();
		}
	);

	EventManager::GetInstance()->Subscribe<void>(
		EventType::HIDE_COUNT_UI,
		[this](std::shared_ptr<void> data)
		{
			HideCountUI();
		}
	);

	using resultArgs = std::tuple<int, int, int>;
	EventManager::GetInstance()->Subscribe<resultArgs>(
		EventType::SHOW_RESULT_UI,
		[this](std::shared_ptr<resultArgs> data)
		{
			ShowResultUI(
				std::get<0>(*data),
				std::get<1>(*data),
				std::get<2>(*data)
			);
		}
	);

	using bulletArgs = std::tuple<int, DirectX::XMFLOAT3, DirectX::XMFLOAT3>;
	EventManager::GetInstance()->Subscribe<bulletArgs>(
		EventType::SET_PLAYER_CHASING_UI_POSITION,
		[this](std::shared_ptr<bulletArgs> data)
		{
			SetPlayerChasingUIPosition(
				std::get<0>(*data),
				std::get<1>(*data),
				std::get<2>(*data)
			);
		}
	);

	using bulletBoolArgs = std::pair<int, bool>;
	EventManager::GetInstance()->Subscribe<bulletBoolArgs>(
		EventType::SET_BULLET_UI_ACTIVE,
		[this](std::shared_ptr<bulletBoolArgs> data)
		{
			SetBulletCountActive(
				std::get<0>(*data),
				std::get<1>(*data)
			);
		}
	);

	EventManager::GetInstance()->Subscribe<void>(
		EventType::INACTIVATE_PLAYER_POINTER_IMAGES,
		[this](std::shared_ptr<void> data)
		{
			m_pPlayerPointerUI->SetActive(false);
		}
	);

	using eventType = EventType;
	EventManager::GetInstance()->Subscribe<eventType>(
		EventType::SHOW_ANOUNCE_UI,
		[this](std::shared_ptr<eventType> data)
		{
			m_pEventAnounceUI->ShowAnounce(*data);
		}
	);

	using boolArgs = bool;
	EventManager::GetInstance()->Subscribe<boolArgs>(
		EventType::ANOUNCE_GOAL,
		[this](std::shared_ptr<boolArgs> data)
		{
			m_showGoalAounce = *data;
		}
	);

	EventManager::GetInstance()->Subscribe<boolArgs>(
		EventType::SET_BB_SCENE_EFFECT,
		[this](std::shared_ptr<boolArgs> data)
		{
			m_pBBEffect->SetActive(*data);
		}
	);

	using pointerInt = int;
	EventManager::GetInstance()->Subscribe<pointerInt>(
		EventType::SET_PLAYER_POINTER_ACTIVE,
		[this](std::shared_ptr<pointerInt> data)
		{
			m_pPlayerPointerUI->SetPlayerActive(*data, true);
		}
	);


	//チームキャラクター設定
	m_pTeamUI1->SetTeamCharacter(
		m_pSceneContext->playersInfo[0].characterID,
		m_pSceneContext->playersInfo[1].characterID
	);
	m_pTeamUI2->SetTeamCharacter(
		m_pSceneContext->playersInfo[2].characterID,
		m_pSceneContext->playersInfo[3].characterID
	);

	m_showGoalAounce = false;
}

//更新
void GameUIManager::UpdateOverride()
{
	const float disolveSpeed = 0.15f;

	auto color = m_pGoalAnounceImage->GetColor();

	if (m_showGoalAounce)
	{
		color.w = std::min(color.w + disolveSpeed, 1.0f);
	}
	else
	{
		color.w = std::max(color.w - disolveSpeed, 0.0f);
	}

	m_pGoalAnounceImage->SetColor(color);

}

//終了
void GameUIManager::FinalizeOverride()
{
}

void GameUIManager::SetPlayerChasingUIPosition(int teamID, const DirectX::XMFLOAT3& position1, const DirectX::XMFLOAT3& position2)
{
	const float offsetY = 90.0f; // Y座標のオフセット値

	//取得座標をスクリーン座標に変換
	DirectX::XMFLOAT2 screenPos1 = m_pCameraInfo->ConvertWorldToScreen(position1, m_screenWidth, m_screenHeight);
	DirectX::XMFLOAT2 screenPos2 = m_pCameraInfo->ConvertWorldToScreen(position2, m_screenWidth, m_screenHeight);
	DirectX::XMFLOAT3 adjustedPosition1 = DirectX::XMFLOAT3{ screenPos1.x, screenPos1.y + offsetY, 0.0f };
	DirectX::XMFLOAT3 adjustedPosition2 = DirectX::XMFLOAT3{ screenPos2.x, screenPos2.y + offsetY, 0.0f };
	if (teamID == 0)
	{
		m_pBulletCountUI1[0]->SetLocalPosition(adjustedPosition1);
		m_pBulletCountUI1[1]->SetLocalPosition(adjustedPosition2);
		m_pPlayerPointerUI->SetPlayerPosition(0, adjustedPosition1);
		m_pPlayerPointerUI->SetPlayerPosition(1, adjustedPosition2);
	}
	else if (teamID == 1)
	{
		m_pBulletCountUI2[0]->SetLocalPosition(adjustedPosition1);
		m_pBulletCountUI2[1]->SetLocalPosition(adjustedPosition2);
		m_pPlayerPointerUI->SetPlayerPosition(2, adjustedPosition1);
		m_pPlayerPointerUI->SetPlayerPosition(3, adjustedPosition2);
	}
}

//HP変更時の処理
void GameUIManager::OnHPChanged(int teamID, float newHP)
{
	if (teamID == 0)
	{
		m_pTeamUI1->GetDamage(newHP);
	}
	else if (teamID == 1)
	{
		m_pTeamUI2->GetDamage(newHP);
	}
}

//弾数変更時の処理
void GameUIManager::OnBulletCountChanged(int teamID, int newCount)
{
	if (teamID == 0)
	{
		for(auto& bulletUI : m_pBulletCountUI1)
		{
			bulletUI->SetBulletCount(newCount);
		}
	}
	else if (teamID == 1)
	{
		for(auto& bulletUI : m_pBulletCountUI2)
		{
			bulletUI->SetBulletCount(newCount);
		}
	}
}

//カットイン呼び出し関数
void GameUIManager::CallCutIn(int teamID, int characterIndex)
{
	if (teamID == 0)
	{
		m_pCutInUI1->CallCutIn(characterIndex);
	}
	else if (teamID == 1)
	{
		m_pCutInUI2->CallCutIn(characterIndex);
	}
}

//カウントUI表示関数
void GameUIManager::ShowCountUI(int count)
{
	m_pCountUI->ShowCountDown(count);
}

//スタートUI表示関数
void GameUIManager::ShowStartUI()
{
	m_pCountUI->ShowStart();
}

//フィニッシュUI表示関数
void GameUIManager::ShowFinishUI()
{
	m_pCountUI->ShowFinish();
}

//カウントUI非表示関数
void GameUIManager::HideCountUI()
{
	m_pCountUI->HideAll();
}

//リザルトUI表示関数
void GameUIManager::ShowResultUI(int winner, int character1ID, int character2ID)
{
	m_pResultUI->SetActive(true);
	m_pResultUI->ShowResult(winner, character1ID, character2ID);
}

//弾数UIアクティブ設定関数
void GameUIManager::SetBulletCountActive(int teamID, bool isActive)
{
	if (teamID == 0)
	{
		for(auto& bulletUI : m_pBulletCountUI1)
		{
			bulletUI->SetActive(isActive);
		}
	}
	else if (teamID == 1)
	{
		for(auto& bulletUI : m_pBulletCountUI2)
		{
			bulletUI->SetActive(isActive);
		}
	}
}