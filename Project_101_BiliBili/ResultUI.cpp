#include "ResultUI.h"
#include "Random.h"
#include <DirectXMath.h>

using namespace DirectX;

//コンストラクタ
ResultUI::ResultUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
	const std::wstring backgroundTexturePaths[2] = {
		L"asset/texture/result_scene/UI_RESULT_Back_B.png",
		L"asset/texture/result_scene/UI_RESULT_Back_R.png"
	};
	const std::wstring footerTexturePaths[2] = {
		L"asset/texture/result_scene/UI_RESULT_Footer_blue.png",
		L"asset/texture/result_scene/UI_RESULT_Footer_red.png"
	};
	const std::wstring texturePaths[2] = {
		L"asset/texture/result_scene/UI_RESULT_Blue_text.png",
		L"asset/texture/result_scene/UI_RESULT_Red_text.png"
	};
	const std::wstring characterTexturePaths[2][4] = {
		{
			L"asset/texture/result_scene/UI_RESULT_character_B_1.png",
			L"asset/texture/result_scene/UI_RESULT_character_B_2.png",
			L"asset/texture/result_scene/UI_RESULT_character_B_3.png",
			L"asset/texture/result_scene/UI_RESULT_character_B_4.png"
		},
		{
			L"asset/texture/result_scene/UI_RESULT_character_R_1.png",
			L"asset/texture/result_scene/UI_RESULT_character_R_2.png",
			L"asset/texture/result_scene/UI_RESULT_character_R_3.png",
			L"asset/texture/result_scene/UI_RESULT_character_R_4.png"
		}
	};
	for (int i = 0; i < 2; ++i)
	{
		//背景画像UI生成
		m_pBackgroundImage[i] = AddChild<UIImage>(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 1920.0f, 1080.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
			0,												//描画順序
			backgroundTexturePaths[i],						//テクスチャパス
			PSO_KEY_TRANSPARENT								//ブレンドモード
		);
		m_pBackgroundImage[i]->SetActive(false); // 初期状態では非表示にする

		//フッター画像UI生成
		m_pFooterImage[i] = AddChild<UIImage>(
			DirectX::XMFLOAT3{ 0.0f, -540.0f + 254.0f * 0.5f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 1920.0f, 296.0f, 1.0f },			//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
			3,													//描画順序
			footerTexturePaths[i],								//テクスチャパス
			PSO_KEY_TRANSPARENT						//ブレンドモード
		);
		m_pFooterImage[i]->SetActive(false); // 初期状態では非表示にする

		//チームテキスト画像UI生成
		const float sign = (i == 0) ? -1.0f : 1.0f;
		const float texPosX = sign * (1980.0f / 2.0f - 120.0f) + -sign * (993.0f / 2.0f + 0.0f);
		m_pTeamTextImage[i] = AddChild<UIImage>(
			DirectX::XMFLOAT3{ texPosX, -500.0f + 185.0f * 0.5f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 993.0f, 185.0f, 1.0f },	//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
			4,													//描画順序
			texturePaths[i],									//テクスチャパス
			PSO_KEY_TRANSPARENT						//ブレンドモード
		);
		m_pTeamTextImage[i]->SetActive(false); // 初期状態では非表示にする

		//キャラクター画像UI生成
		const float scaleFactor = 1.7f;
		for (int j = 0; j < 4; ++j)
		{
			m_pCharacterImage[i][j] = AddChild<UIImage>(
				DirectX::XMFLOAT3{ 0.0f, -0.0f, 0.0f },			//位置
				DirectX::XMFLOAT3{ 720.0f * scaleFactor, 1080.0f * scaleFactor, 1.0f },			//スケール
				DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
				2,												//描画順序
				characterTexturePaths[i][j],					//テクスチャパス
				PSO_KEY_TRANSPARENT						//ブレンドモード
			);
			m_pCharacterImage[i][j]->SetActive(false); // 初期状態では非表示にする
		}
	}

	m_pHeaderImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 540.0f - 175.0f * 0.5f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 1920.0f, 175.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		5,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Header.png",	//テクスチャパス
		PSO_KEY_TRANSPARENT						//ブレンドモード
	);

	const float itemScaleFactor = 0.2f;
	TexSplitInfo texInfo{};
	texInfo.cols = 6;
	texInfo.rows = 5;
	texInfo.total = 30;
	texInfo.index = 0;
	texInfo.frameCount = 0;
	texInfo.updateRate = 2;
	m_pItemImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 100.0f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 								//スケール
			3072.0f / static_cast<float>(texInfo.cols),
			2560.0f / static_cast<float>(texInfo.rows),
			1.0f
		},
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
		1,												//描画順序
		L"asset/texture/game_scene/change_item.png",	//テクスチャパス
		PSO_KEY_TRANSPARENT									//ブレンドモード
	);
	m_pItemImage->SetTexSplitInfo(texInfo);

	m_pGoToTitleImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 700.0f, 400.0f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 449.0f, 80.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		5,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Title.png",	//テクスチャパス
		PSO_KEY_TRANSPARENT						//ブレンドモード
	);
	m_pGoToTitleImageWrap = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 700.0f, 400.0f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 424.0f, 55.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		6,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Title_ON.png",	//テクスチャパス
		PSO_KEY_TRANSPARENT						//ブレンドモード
	);

	for(int i = 0; i < 100; ++i)
	{
		//紙吹雪画像UI生成
		m_pConfettiImage[i] = AddChild<UIImage>(
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//位置
			DirectX::XMFLOAT3{ 30.0f, 30.0f, 1.0f },		//スケール
			DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
			100,												//描画順序
			L"asset/texture/white.png",	//テクスチャパス
			PSO_KEY_TRANSPARENT						//ブレンドモード
		);
		m_pConfettiImage[i]->SetActive(false); // 初期状態では非表示にする
		m_pConfettiImage[i]->SetColor({ 1.0f, 1.0f, 1.0f, 0.5f });
	}

	m_pThankyouForPlaying = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 1080.0f, 0.0f },					//位置
		DirectX::XMFLOAT3{ 1920.0f, 1080.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },					//回転
		102,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Shutter.png",	//テクスチャパス
		PSO_KEY_TRANSPARENT										//ブレンドモード
	);

	m_pSparkleImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ -750.0f, 450.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ 500.0f, 500.0f, 1.0f },		//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },			//回転
		101,											//描画順序
		L"asset/texture/result_scene/kirakira_EF.png",	//テクスチャパス
		PSO_KEY_ADDITIVE								//ブレンドモード
	);
	TexSplitInfo sparkleTexInfo{};
	sparkleTexInfo.cols = 6;
	sparkleTexInfo.rows = 5;
	sparkleTexInfo.total = 30;
	sparkleTexInfo.index = 0;
	sparkleTexInfo.frameCount = 0;
	sparkleTexInfo.updateRate = 2;
	m_pSparkleImage->SetTexSplitInfo(sparkleTexInfo);

	//全ての子UIを非アクティブに設定
	for(auto& child : m_children)
	{
		child->SetActive(false); //全ての子UIを非アクティブに設定
	}

	m_pRandom = new Random(1);
}

//初期化
void ResultUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//更新
void ResultUI::UpdateOverride()
{
	m_mainTimer++;	//メインタイマー更新

	if (m_isResultShown)
	{
		const int SHOW_UI_TIME = 90;
		const int SHOW_GO_TO_TITLE = 150;

		if (m_mainTimer == SHOW_UI_TIME)
		{
			m_pCharacterImage[m_winner][m_character1ID]->SetActive(true);
			m_pCharacterImage[m_winner][m_character2ID]->SetActive(true);
			m_pFooterImage[m_winner]->SetActive(true);
			m_pTeamTextImage[m_winner]->SetActive(true);
			m_pItemImage->SetActive(true);
			m_pGoToTitleImage->SetActive(true);
			m_pGoToTitleImageWrap->SetActive(true);
			m_confettiActive = true; 
			m_confettiTimer = 0;
		}

		if (m_mainTimer >= SHOW_GO_TO_TITLE)
		{
			auto color = m_pGoToTitleImage->GetColor();
			color.w = (std::min)(color.w + 0.01f, 1.0f);
			m_pGoToTitleImage->SetColor(color);
			m_isGoToTitleShown = true;
		}
	}

	//タイトルへ戻るボタンのUVとスケール、位置を更新
	auto uvRect =  m_pGoToTitleImageWrap->GetUVRect();
	uvRect.sv = m_goToTitlePressAmount;
	uvRect.v = 1.0f - m_goToTitlePressAmount;
	m_pGoToTitleImageWrap->SetUVRect(uvRect);
	auto scale = m_pGoToTitleImageWrap->GetLocalScale();
	scale.y = 55.0f * m_goToTitlePressAmount;
	m_pGoToTitleImageWrap->SetLocalScale(scale);
	auto position = m_pGoToTitleImageWrap->GetLocalPosition();
	position.y = (55.0f / 2.0f) * m_goToTitlePressAmount + (400.0f - 27.5f);
	m_pGoToTitleImageWrap->SetLocalPosition(position);

	if(m_shakeGoToTitleButton)
	{
		const float shakeAmount = 3.0f; //揺れの強さ
		XMFLOAT3 offset{};
		offset.x = m_pRandom->GetFloat(-shakeAmount, shakeAmount);
		offset.y = m_pRandom->GetFloat(-shakeAmount, shakeAmount);
		m_pGoToTitleImage->SetDrawOffset(offset);
		m_pGoToTitleImageWrap->SetDrawOffset(offset);
	}
	else
	{
		m_pGoToTitleImage->SetDrawOffset({ 0.0f, 0.0f, 0.0f });
		m_pGoToTitleImageWrap->SetDrawOffset({ 0.0f, 0.0f, 0.0f });
	}
	m_shakeGoToTitleButton = false; //揺らすフラグリセット

	if(m_pThankyouForPlaying->IsActive())
	{
		const int SCROLL_DURATION = 10;	//スクロールにかかるフレーム数
		float t = m_ThankYouForPlayingFallTimer / static_cast<float>(SCROLL_DURATION);
		t = std::clamp(t, 0.0f, 1.0f);
		auto position = m_pThankyouForPlaying->GetLocalPosition();
		position.y = Lerpf(1080.0f, 0.0f, t);	//Y位置を線形補間で更新
		m_pThankyouForPlaying->SetLocalPosition(position);

		m_ThankYouForPlayingFallTimer++;
	}

	//紙吹雪更新
	UpdateConfetti();
}

//終了
void ResultUI::FinalizeOverride()
{
}

void ResultUI::ShowResult(int winner, int character1ID, int character2ID)
{
	//無効な勝利チームIDまたはキャラクターIDの場合は処理を中断
	if (character1ID < 0 || character1ID >= MAX_CHARACTER_NUM
	 || character2ID < 0 || character2ID >= MAX_CHARACTER_NUM)
	{
		return; //無効なキャラクターIDの場合は処理を中断
	}

	m_winner = winner;
	m_character1ID = character1ID;
	m_character2ID = character2ID;
	m_isGoToTitleShown = false; //タイトルへ戻る表示フラグOFF

	for(auto& child : m_children)
	{
		child->SetActive(false);
	}

	m_pHeaderImage->SetActive(true); //ヘッダー画像を表示

	//勝利チームの背景、フッター、チームテキスト、キャラクター画像を表示
	m_pBackgroundImage[m_winner]->SetActive(true);
	m_pSparkleImage->SetActive(true);

	DirectX::XMFLOAT3 cnaracterPosition1{};
	DirectX::XMFLOAT3 cnaracterPosition2{};

	UIImage& char1Image = *m_pCharacterImage[m_winner][character1ID];
	UIImage& char2Image = *m_pCharacterImage[m_winner][character2ID];

	const XMFLOAT3 characterPositionsL[MAX_CHARACTER_NUM] = {
		{ -1.0f, -1.0f, -1.0f  },	// CHARACTER_GREEN(未使用)
		{ -350.0f, -350.0f, 0.0f },	// CHARACTER_BLUE
		{ -300.0f, -350.0f, 0.0f },	// CHARACTER_YELLOW
		{ -400.0f, -350.0f, 0.0f }	// CHARACTER_RED
	};

	const XMFLOAT3 characterPositionsR[MAX_CHARACTER_NUM] = {
		{ 450, -480.0f, 0.0f  },	// CHARACTER_GREEN
		{ 600.0f, -350.0f, 0.0f },	// CHARACTER_BLUE
		{ 600.0f, -350.0f, 0.0f },	// CHARACTER_YELLOW
		{ 600.0f, -350.0f, 0.0f }	// CHARACTER_RED
	};

	//位置設定
	if(character1ID == CHARACTER_LUMINA || character2ID == CHARACTER_LUMINA)
	{//LUMINAが含まれる場合の初期位置設定
		//LUMINAが右側に来るように位置を設定
		if(character1ID == CHARACTER_LUMINA)
		{
			cnaracterPosition1 = characterPositionsR[CHARACTER_LUMINA];
			cnaracterPosition2 = characterPositionsL[character2ID];
		}
		else
		{
			cnaracterPosition1 = characterPositionsL[character1ID];
			cnaracterPosition2 = characterPositionsR[CHARACTER_LUMINA];
		}
	}
	else
	{//LUMINAが含まれない場合の位置設定
		//character1IDを左側、character2IDを右側に設定
		cnaracterPosition1 = characterPositionsL[character1ID];
		cnaracterPosition2 = characterPositionsR[character2ID];
	}

	char1Image.SetLocalPosition(cnaracterPosition1);
	char2Image.SetLocalPosition(cnaracterPosition2);

	m_isResultShown = true; //リザルト表示フラグON
	m_mainTimer = 0; //メインタイマーリセット

	m_pThankyouForPlaying->SetActive(false); // "Thank you for playing"を非表示にする
}

void ResultUI::ShakeGoToTitleButton()
{
	m_shakeGoToTitleButton = true;
}

//オブジェクトの描画情報生成
void ResultUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	
}

void ResultUI::UpdateConfetti()
{
	if (!m_confettiActive) return;

	const int CONFETTI_GENERATE_INTERVAL = 10; //紙吹雪生成間隔フレーム数
	if (m_confettiTimer >= CONFETTI_GENERATE_INTERVAL)
	{

		for (int i = 0; i < 100; ++i)
		{
			auto confetti = m_pConfettiImage[i];

			if (!confetti->IsActive())
			{
				auto position = confetti->GetLocalPosition();
				position.x = m_pRandom->GetFloat(-1000.0f, 1000.0f);
				position.y = 600.0f; //Y方向の移動量
				position.z = 0.0f; //Z方向の移動量
				confetti->SetLocalPosition(position);
				confetti->SetLocalRotation(XMFLOAT3{ m_pRandom->GetFloat(-30.0f, 30.0f), m_pRandom->GetFloat(-30.0f, 30.0f), m_pRandom->GetFloat(-30.0f, 30.0f) });
				float scale = m_pRandom->GetFloat(20.0f, 80.0f);
				confetti->SetLocalScale(XMFLOAT3{ scale, scale, 1.0f });
				auto color = confetti->GetColor();
				color = XMFLOAT4(1.0f, 1.0f, 1.0f, 0.5f);
				confetti->SetColor(color);
				confetti->SetActive(true);
				break;
			}
		}

		m_confettiTimer = 0;
	}

	for(int i = 0; i < 100; ++i)
	{
		auto confetti = m_pConfettiImage[i];

		if(confetti->IsActive())
		{
			auto scale = confetti->GetLocalScale();
			float fallSpeed = scale.x * 0.15f; //紙吹雪の大きさに応じた落下速度

			//紙吹雪の位置を更新
			auto position = confetti->GetLocalPosition();
			position.x += position.x * 0.002f; //X方向の移動量
			position.y -= fallSpeed; //Y方向の移動量
			position.z += 0.0f; //Z方向の移動量
			confetti->SetLocalPosition(position);

			//紙吹雪の回転を更新
			auto rotation = confetti->GetLocalRotation();
			rotation.x += 1.1f; //X軸回転量
			rotation.y += 1.2f; //Y軸回転量
			confetti->SetLocalRotation(rotation);

			//紙吹雪の色を更新
			auto color = confetti->GetColor();
			color.z -= 0.002f;
			confetti->SetColor(color);

			//画面外に出たら非表示にする
			if(position.y < -600.0f)
			{
				confetti->SetActive(false);
			}
		}
	}

	m_confettiTimer++;
}
