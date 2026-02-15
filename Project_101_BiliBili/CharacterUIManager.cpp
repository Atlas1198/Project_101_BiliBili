#include "CharacterUIManager.h"
#include "EventManager.h"

using namespace DirectX;

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
	//背景画像UIオブジェクト生成
	m_pBackgroundImage = new UIImage(
		{ 0.0f, 0.0f, 0.0f },	//位置
		{ m_screenWidth, m_screenWidth / 1421.0f * m_screenHeight, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		0,						//描画順序
		L"asset/texture/character_scene/UI_CHARACTER_Back.png",
		PSO_KEY_OPAQUE
	);
	//ルートUIオブジェクトに追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pBackgroundImage));

	//ヘッダー画像UIオブジェクト生成
	m_pHeaderImage = new UIImage(
		{ 0.0f, m_screenHeight * 0.5f - 76.5f, 0.0f },	//位置
		{ m_screenWidth, 153.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		1,						//描画順序
		L"asset/texture/character_scene/UI_CHARACTER_Header.png",
		PSO_KEY_TRANSPARENT
	);
	//ルートUIオブジェクトに追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pHeaderImage));

	//キャラクターアイコン関連定数
	const float characterIconPosY = -100.0f;	//キャラクターアイコンY位置
	const XMFLOAT3 characterIconScale = { 474.0f, 732.0f, 1.0f };	//キャラクターアイコンスケール
	const float characterIconBaseScaleX = 1909.0f;	//キャラクターアイコンベースUIオブジェクトのXスケール
	const float positionOffsetX = (characterIconBaseScaleX - characterIconScale.x * 4) / 3.0f; //キャラクターアイコン間のX位置オフセット
	const float iconScaleFactor = 0.99f; //キャラクターアイコンスケール倍率

	//プレイヤー背景UIオブジェクト配列生成
	{
		const float basePositionX = -characterIconBaseScaleX * 0.5f + characterIconScale.x * 0.5f;

		for (int i = 0; i < 4; ++i)
		{
			float positionX = basePositionX + i * (characterIconScale.x + positionOffsetX);

			m_pPlayerBackgroundsNormal[i] = new UIImage(
				{ positionX, characterIconPosY, 0.0f },	//位置
				XMFLOAT3(characterIconScale.x * iconScaleFactor, characterIconScale.y * iconScaleFactor, 1.0f),//スケール
				{ 0.0f, 0.0f, 0.0f },	//回転
				2,						//描画順序
				L"asset/texture/character_scene/UI_CHARACTER_Select_N.png",
				PSO_KEY_MASKED
			);
			//ルートUIオブジェクトに追加
			m_roots.push_back(std::unique_ptr<UIBase>(m_pPlayerBackgroundsNormal[i]));
		}
	}

	//プレイヤー背景UIオブジェクト配列生成
	{
		m_pPlayerBackgroundsSelected[0] = new UIImage(
			{ 0.0f, characterIconPosY, 0.0f },	//位置
			XMFLOAT3(characterIconScale.x * iconScaleFactor, characterIconScale.y * iconScaleFactor, 1.0f),//スケール
			{ 0.0f, 0.0f, 0.0f },	//回転
			2,						//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Select_1.png",
			PSO_KEY_MASKED
		);
		m_pPlayerBackgroundsSelected[1] = new UIImage(
			{ 0.0f, characterIconPosY, 0.0f },	//位置
			XMFLOAT3(characterIconScale.x * iconScaleFactor, characterIconScale.y * iconScaleFactor, 1.0f),//スケール
			{ 0.0f, 0.0f, 0.0f },	//回転
			2,						//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Select_2.png",
			PSO_KEY_MASKED
		);
		m_pPlayerBackgroundsSelected[2] = new UIImage(
			{ 0.0f, characterIconPosY, 0.0f },	//位置
			XMFLOAT3(characterIconScale.x * iconScaleFactor, characterIconScale.y * iconScaleFactor, 1.0f),//スケール
			{ 0.0f, 0.0f, 0.0f },	//回転
			2,						//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Select_3.png",
			PSO_KEY_MASKED
		);
		m_pPlayerBackgroundsSelected[3] = new UIImage(
			{ 0.0f, characterIconPosY, 0.0f },	//位置
			XMFLOAT3(characterIconScale.x * iconScaleFactor, characterIconScale.y * iconScaleFactor, 1.0f),//スケール
			{ 0.0f, 0.0f, 0.0f },	//回転
			2,						//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Select_4.png",
			PSO_KEY_MASKED
		);
		//ルートUIオブジェクトに追加
		for (auto& bg : m_pPlayerBackgroundsSelected)
		{
			//ルートUIオブジェクトに追加
			m_roots.push_back(std::unique_ptr<UIBase>(bg));
			bg->SetActive(false);
		}
	}

	//キャラクターアイコンベースUIオブジェクト生成
	m_pCharacterIconBase = new UIImage(
		{ 0.0f, characterIconPosY, 0.0f },	//位置
		{ characterIconBaseScaleX, characterIconScale.y, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		3,						//描画順序
		L"asset/texture/character_scene/UI_CHARACTER_Select_frame.png",
		PSO_KEY_MASKED
	);
	//ルートUIオブジェクトに追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pCharacterIconBase));

	//プレイヤーブラーUIオブジェクト配列生成
	{
		std::wstring playerBlurTexturePaths[4] =
		{
			L"asset/texture/character_scene/UI_CHARACTER_Blur_1.png",
			L"asset/texture/character_scene/UI_CHARACTER_Blur_2.png",
			L"asset/texture/character_scene/UI_CHARACTER_Blur_3.png",
			L"asset/texture/character_scene/UI_CHARACTER_Blur_4.png",
		};

		const float basePositionX = -characterIconBaseScaleX * 0.5f + characterIconScale.x * 0.5f;
		for (int i = 0; i < 4; ++i)
		{
			float positionX = basePositionX + i * (characterIconScale.x + positionOffsetX);
			const float scaleFactor = 1.1f; //ブラースケール倍率
			m_pPlayerBlurs[i] = new UIImage(
				{ positionX, characterIconPosY, 0.0f },	//位置
				XMFLOAT3(468.0f * scaleFactor, 707.0f * scaleFactor, 1.0f),//スケール
				{ 0.0f, 0.0f, 0.0f },	//回転
				4,						//描画順序
				playerBlurTexturePaths[i],
				PSO_KEY_ADDITIVE
			);
			m_pPlayerBlurs[i]->SetActive(false);
			//ルートUIオブジェクトに追加
			m_roots.push_back(std::unique_ptr<UIBase>(m_pPlayerBlurs[i]));
		}
	}

	//プレイヤーアイコンUIオブジェクト配列生成
	{
		const XMFLOAT3 iconScale = { 88.0f, 107.0f, 1.0f };
		float basePositionY = m_pCharacterIconBase->GetLocalPosition().y;
		float baseScaleY = m_pCharacterIconBase->GetLocalScale().y;
		float positionY = basePositionY + baseScaleY * 0.5f + iconScale.y * 0.5f;

		const float positionOffsetXForIcon = 20.0f; //キャラクターアイコン間のX位置オフセット
		float basePositionX = -characterIconBaseScaleX * 0.5f + characterIconScale.x * 0.5f - iconScale.x * 1.5f - positionOffsetXForIcon * 1.5f;

		m_pPlayerIcons[0] = new UIImage(
			{ 0.0f, positionY, 0.0f },	//位置
			iconScale,						//スケール
			{ 0.0f, 0.0f, 0.0f },			//回転
			5,								//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Icon_1.png",
			PSO_KEY_TRANSPARENT
		);
		m_pPlayerIcons[1] = new UIImage(
			{ 0.0f, positionY, 0.0f },	//位置
			iconScale,						//スケール
			{ 0.0f, 0.0f, 0.0f },			//回転
			5,								//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Icon_2.png",
			PSO_KEY_TRANSPARENT
		);
		m_pPlayerIcons[2] = new UIImage(
			{ 0.0f, positionY, 0.0f },	//位置
			iconScale,						//スケール
			{ 0.0f, 0.0f, 0.0f },			//回転
			5,								//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Icon_3.png",
			PSO_KEY_TRANSPARENT
		);
		m_pPlayerIcons[3] = new UIImage(
			{ 0.0f, positionY, 0.0f },	//位置
			iconScale,						//スケール
			{ 0.0f, 0.0f, 0.0f },			//回転
			5,								//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Icon_4.png",
			PSO_KEY_TRANSPARENT
		);

		//初期X座標設定
		for (int i = 0; i < 4; ++i)
		{
			float positionX = basePositionX + i * iconScale.x + positionOffsetXForIcon * i;
			auto position = m_pPlayerIcons[i]->GetLocalPosition();
			position.x = positionX;
			m_pPlayerIcons[i]->SetLocalPosition(position);
		}

		//プレイヤーアイコンのXスケールオフセット計算
		for(int i = 0; i < 4; ++i)
		{
			float centerX = m_pPlayerBackgroundsNormal[0]->GetLocalPosition().x;
			float sub = m_pPlayerIcons[i]->GetLocalPosition().x - centerX;
			playerIconScaleOffsets[i] = sub;
		}

		//ルートUIオブジェクトに追加
		for (auto& icon : m_pPlayerIcons)
		{
			//ルートUIオブジェクトに追加
			m_roots.push_back(std::unique_ptr<UIBase>(icon));
		}
	}

	//操作説明UIオブジェクト生成
	m_pOperation = new UIImage(
		{ 0.0f, -m_screenHeight * 0.5f + 38.5f, 0.0f },	//位置
		{ 1917.0f, 77.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		5,						//描画順序
		L"asset/texture/character_scene/UI_CHARACTER_Operation.png",
		PSO_KEY_TRANSPARENT
	);
	//ルートUIオブジェクトに追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pOperation));

	m_pLoadUI = new LoadUI(
		{ 0.0f, 0.0f, 0.0f },	//位置
		{ 1.0f, 1.0f, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		100						//描画順序
	);
	m_pLoadUI->SetActive(false);
	//ルートUIオブジェクトに追加
	m_roots.push_back(std::unique_ptr<UIBase>(m_pLoadUI));

	{
		//次のシーンへ進むアイコン位置計算
		const XMFLOAT3 position =
		{
			-m_screenWidth * 0.5f - 1917.0f * 0.5f,
			-m_screenHeight * 0.5f + 186.0f * 0.5f,
			0.0f
		};

		//次のシーンへ進むアイコンUIオブジェクト生成
		m_pGoToNextSceneIcon = new UIImage(
			position,					//位置
			{ 1917.0f, 186.0f, 1.0f },	//スケール
			{ 0.0f, 0.0f, 0.0f },		//回転
			5,							//描画順序
			L"asset/texture/character_scene/UI_CHARACTER_Ready.png",
			PSO_KEY_TRANSPARENT
		);
		//ルートUIオブジェクトに追加
		m_roots.push_back(std::unique_ptr<UIBase>(m_pGoToNextSceneIcon));
	}

	//イベント購読登録
	using args = std::pair<int, int>;
	EventManager::GetInstance()->Subscribe<args>(
		EventType::CHARACTER_ICON_MOVE,
		[this](std::shared_ptr<args> data)
		{
			MoveCharacterIcon(data->first, data->second);
		}
	);
	using args2 = std::pair<int, int>;
	EventManager::GetInstance()->Subscribe<args2>(
		EventType::SHOW_SELECTED_ICON,
		[this](std::shared_ptr<args2> data)
		{
			ActivateSelectedPlayerBackground(data->first, data->second);
		}
	);
	EventManager::GetInstance()->Subscribe<args2>(
		EventType::HIDE_SELECTED_ICON,
		[this](std::shared_ptr<args2> data)
		{
			DeactivateSelectedPlayerBackground(data->first, data->second);
		}
	);
	EventManager::GetInstance()->Subscribe<void>(
		EventType::GO_TO_GAME_SCENE,
		[this](std::shared_ptr<void> data)
		{
			GoToNextScene();
		}
	);

	EventManager::GetInstance()->Subscribe<void>(
		EventType::SHOW_LOAD_UI,
		[this](std::shared_ptr<void> data)
		{
			m_pLoadUI->SetActive(true);
		}
	);

	//メンバ変数初期化
	m_passedFrameCount = 0.0f;
	for(auto& selected : m_playersSelected)
	{
		selected = false;
	}
	m_isGoToNextSceneIconVisible = false;
	m_goToNextSceneIconMoveCount = 0;
	m_isCalledGoToNextScene = false;
}

//更新
void CharacterUIManager::UpdateOverride()
{
	m_passedFrameCount += 1.0f;	//経過フレーム数更新

	//背景スクロール
	{
		UVRect uvRect = m_pBackgroundImage->GetUVRect();
		uvRect.u += 0.002f;
		m_pBackgroundImage->SetUVRect(uvRect);
	}

	//プレイヤーアイコンのサイズ変更
	{
		float scaleFactor = 1.0f + 0.15f * std::sin(static_cast<float>(m_passedFrameCount) * 0.08f);

		for (int i = 0; i < 4; ++i)
		{
			if (m_pPlayerIcons[i]->IsActive() && !m_playersSelected[i])
			{
				m_pPlayerIcons[i]->SetLocalScale({ 88.0f * scaleFactor, 107.0f * scaleFactor, 1.0f });
			}
		}
	}

	//次のシーンへ進むアイコンの座標移動
	{
		auto position = m_pGoToNextSceneIcon->GetLocalPosition();
		const float moveSpeedCoefficient = 100.0f - position.x;	//移動速度係数計算
		const float moveSpeed = moveSpeedCoefficient * 0.2f;	//移動速度計算

		if(m_isGoToNextSceneIconVisible)
		{//表示中はX座標を中心に移動
			position.x = (std::min)(position.x + moveSpeed, 0.0f);
			m_pGoToNextSceneIcon->SetLocalPosition(position);
		}
		else
		{//非表示中は画面外へ移動
			position.x = (std::max)(position.x - moveSpeed, -m_screenWidth * 0.5f - 1917.0f * 0.5f);
			m_pGoToNextSceneIcon->SetLocalPosition(position);
		}
	}

	//次のシーンへ進むアイコンの拡縮
	if (m_isCalledGoToNextScene)
	{
		m_goToNextSceneIconMoveCount++;
		const float changeAmount = 1.07f;
		if(m_goToNextSceneIconMoveCount <= 3)
		{//拡大
			auto scale = m_pGoToNextSceneIcon->GetLocalScale();
			scale.x *= changeAmount;
			scale.y *= changeAmount;
			m_pGoToNextSceneIcon->SetLocalScale(scale);
		}
		else if(m_goToNextSceneIconMoveCount <= 6)
		{//縮小
			auto scale = m_pGoToNextSceneIcon->GetLocalScale();
			scale.x /= changeAmount;
			scale.y /= changeAmount;
			m_pGoToNextSceneIcon->SetLocalScale(scale);
		}
	}

	//プレイヤーブラーの更新
	{
		for (int i = 0; i < MAX_CHARACTER_NUM; ++i)
		{
			auto blur = m_pPlayerBlurs[i];
			if (blur->IsActive())
			{
				const float colorDecreaseSpeed = 0.06f;

				auto color = blur->GetColor();
				const float decraseAmount = 1.0f - color.w;
				color.w -= colorDecreaseSpeed;
				if (color.w < 0.0f)
				{
					color.w = 0.0f;
					blur->SetActive(false);
				}
				blur->SetColor(color);

				float scaleX = Lerpf(468.0f * 1.1f, 468.0f, decraseAmount);
				float scaleY = Lerpf(707.0f * 1.1f, 707.0f, decraseAmount);

				auto scale = blur->GetLocalScale();
				scale.x = scaleX;
				scale.y = scaleY;
				blur->SetLocalScale(scale);
			}
		}
	}

	//全プレイヤー選択済みチェック
	bool isAllPlayersSelected = true;
	for(auto selected : m_playersSelected)
	{
		if (!selected)
		{
			isAllPlayersSelected = false;
			break;
		}
	}
	m_isGoToNextSceneIconVisible = isAllPlayersSelected;
	m_pOperation->SetActive(!m_isGoToNextSceneIconVisible);
}

//終了
void CharacterUIManager::FinalizeOverride()
{
}

//キャラクターアイコン移動
void CharacterUIManager::MoveCharacterIcon(int playerIndex, int characterIndex)
{
	//移動量計算
	const float backGroundIconWidth = m_pPlayerBackgroundsNormal[0]->GetLocalScale().x;								//背景アイコンの幅
	const float basePositionX = -m_pCharacterIconBase->GetLocalScale().x * 0.5f + m_pPlayerBackgroundsSelected[0]->GetLocalScale().x * 0.5f;	//ベース位置X
	const float lengthBetweenIcons = (m_pCharacterIconBase->GetLocalScale().x - backGroundIconWidth * 4.0f) / 3.0f;	//アイコン間の長さ
	const float moveAmount = backGroundIconWidth + lengthBetweenIcons;															//移動量

	//移動処理
	auto position = m_pPlayerIcons[playerIndex]->GetLocalPosition();								//現在の変換情報取得
	float newPositionX = basePositionX + characterIndex * moveAmount + playerIconScaleOffsets[playerIndex];	//新しいX位置計算
	position.x = newPositionX;																	//X位置更新

	//変換情報設定
	m_pPlayerIcons[playerIndex]->SetLocalPosition(position);
}

//選択済みプレイヤー背景を有効化
void CharacterUIManager::ActivateSelectedPlayerBackground(int playerIndex, int characterIndex)
{
	//指定インデックスの選択済み背景を有効化
	if (playerIndex >= 0 && playerIndex < MAX_CHARACTER_NUM)
	{
		m_pPlayerBackgroundsSelected[playerIndex]->SetActive(true);
		auto position = m_pPlayerBackgroundsSelected[playerIndex]->GetLocalPosition();
		position = m_pPlayerBackgroundsNormal[characterIndex]->GetLocalPosition();
		m_pPlayerBackgroundsSelected[playerIndex]->SetLocalPosition(position);

		auto scale = m_pPlayerIcons[playerIndex]->GetLocalScale();
		const float iconScaleFactor = 1.2f;
		scale = { 88.0f * iconScaleFactor, 107.0f * iconScaleFactor, 1.0f };
		m_pPlayerIcons[playerIndex]->SetLocalScale(scale);

		m_playersSelected[playerIndex] = true;
		m_pPlayerBlurs[characterIndex]->SetActive(true);
		auto color = m_pPlayerBlurs[characterIndex]->GetColor();
		color.w = 1.0f;
		m_pPlayerBlurs[characterIndex]->SetColor(color);
	}
}

//選択済みプレイヤー背景を無効化
void CharacterUIManager::DeactivateSelectedPlayerBackground(int playerIndex, int characterIndex)
{
	//指定インデックスの選択済み背景を無効化
	if (playerIndex >= 0 && playerIndex < 4)
	{
		m_pPlayerBackgroundsSelected[playerIndex]->SetActive(false);

		auto position = m_pPlayerIcons[playerIndex]->GetLocalPosition();
		auto scale = m_pPlayerIcons[playerIndex]->GetLocalScale();
		auto rotation = m_pPlayerIcons[playerIndex]->GetLocalRotation();
		m_playersSelected[playerIndex] = false;
		m_pPlayerIcons[playerIndex]->SetLocalPosition(position);
		m_pPlayerIcons[playerIndex]->SetLocalScale(scale);
		m_pPlayerIcons[playerIndex]->SetLocalRotation(rotation);
		m_pPlayerBlurs[characterIndex]->SetActive(false);
	}
}

//次のシーンへ進む
void CharacterUIManager::GoToNextScene()
{
	m_isCalledGoToNextScene = true;
	m_goToNextSceneIconMoveCount = 0;
}