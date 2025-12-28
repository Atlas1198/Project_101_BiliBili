#include "CountUI.h"

using namespace DirectX;

//コンストラクタ
CountUI::CountUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
	//カウント3画像UI生成
	m_pCountImage3 = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 421.0f, 513.0f, 1.0f },//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		1,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_3.png"//テクスチャパス
	);
	//カウント2画像UI生成
	m_pCountImage2 = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 446.0f, 499.0f, 1.0f },//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		1,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_2.png"//テクスチャパス
	);
	//カウント1画像UI生成
	m_pCountImage1 = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 421.0f, 513.0f, 1.0f },//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		1,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_1.png"//テクスチャパス
	);
	//スタート画像UI生成
	m_pStartImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 884.0f, 327.0f, 1.0f },//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		1,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_Start.png"//テクスチャパス
	);
	//フィニッシュ画像UI生成
	m_pFinishImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//位置
		DirectX::XMFLOAT3{ 823.0f, 435.0f, 1.0f },//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },	//回転
		1,										//描画順序
		L"asset/texture/game_scene/UI_INGAME_Finish.png"//テクスチャパス
	);

	//全て非アクティブに設定
	m_pCountImage3->SetActive(false);
	m_pCountImage2->SetActive(false);
	m_pCountImage1->SetActive(false);
	m_pStartImage->SetActive(false);
	m_pFinishImage->SetActive(false);

	//カウント画像の基準サイズを保存
	m_countImageBaseSize[0] = m_pCountImage1->GetLocalTransform().scale;
	m_countImageBaseSize[1] = m_pCountImage2->GetLocalTransform().scale;
	m_countImageBaseSize[2] = m_pCountImage3->GetLocalTransform().scale;
}

//初期化
void CountUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//更新
void CountUI::UpdateOverride()
{
	m_mainTimer++;	//メインタイマー更新

	if(m_pCountImage3->IsActive())
	{
		UpdateNumberImage(*m_pCountImage3, m_countImageBaseSize[2]); //数字表示更新
	}
	else if(m_pCountImage2->IsActive())
	{
		UpdateNumberImage(*m_pCountImage2, m_countImageBaseSize[1]); //数字表示更新
	}
	else if(m_pCountImage1->IsActive())
	{
		UpdateNumberImage(*m_pCountImage1, m_countImageBaseSize[0]); //数字表示更新
	}
	else if(m_pStartImage->IsActive())
	{
		UpdateStartImage(*m_pStartImage); //スタート表示更新
	}
	else if(m_pFinishImage->IsActive())
	{
		UpdateFinishImage(*m_pFinishImage); //フィニッシュ表示更新
	}
}

//終了
void CountUI::FinalizeOverride()
{
}

//カウントダウン表示関数(引数:カウント数)
void CountUI::ShowCountDown(int count)
{
	m_mainTimer = 0; //メインタイマーリセット

	//全て非アクティブに設定
	m_pCountImage3->SetActive(false);
	m_pCountImage2->SetActive(false);
	m_pCountImage1->SetActive(false);
	m_pStartImage->SetActive(false);
	m_pFinishImage->SetActive(false);
	//引数のカウント数に応じて対応する画像をアクティブに設定
	switch (count)
	{
	case 3:
		m_pCountImage3->SetActive(true);
		break;
	case 2:
		m_pCountImage2->SetActive(true);
		break;
	case 1:
		m_pCountImage1->SetActive(true);
		break;
	default:
		//それ以外は何もしない
		break;
	}
}

//スタート表示関数
void CountUI::ShowStart()
{
	m_mainTimer = 0; //メインタイマーリセット

	//全て非アクティブに設定
	m_pCountImage3->SetActive(false);
	m_pCountImage2->SetActive(false);
	m_pCountImage1->SetActive(false);
	m_pFinishImage->SetActive(false);
	//スタート画像をアクティブに設定
	m_pStartImage->SetActive(true);
}

//フィニッシュ表示関数
void CountUI::ShowFinish()
{
	m_mainTimer = 0; //メインタイマーリセット

	//全て非アクティブに設定
	m_pCountImage3->SetActive(false);
	m_pCountImage2->SetActive(false);
	m_pCountImage1->SetActive(false);
	m_pStartImage->SetActive(false);
	//フィニッシュ画像をアクティブに設定
	m_pFinishImage->SetActive(true);
}

//全て非表示関数
void CountUI::HideAll()
{
	m_pCountImage3->SetActive(false);
	m_pCountImage2->SetActive(false);
	m_pCountImage1->SetActive(false);
	m_pStartImage->SetActive(false);
	m_pFinishImage->SetActive(false);
}

//オブジェクトの描画情報生成
void CountUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	m_pCountImage3->PrepareRenderInfo(textureManager, meshManager);
	m_pCountImage2->PrepareRenderInfo(textureManager, meshManager);
	m_pCountImage1->PrepareRenderInfo(textureManager, meshManager);
	m_pStartImage->PrepareRenderInfo(textureManager, meshManager);
	m_pFinishImage->PrepareRenderInfo(textureManager, meshManager);
}

//数字表示更新関数
void CountUI::UpdateNumberImage(UIImage& image, XMFLOAT3 baseSize)
{
	float t = Clamp01(m_mainTimer / 60.0f);
	t = EaseOutCubic(t);

	float rotationZ = 0.0f;
	rotationZ = Lerpf(-30.0f, 10.0f, t);

	XMFLOAT3 scale = { 0.0f, 0.0f, 0.0f };
	XMFLOAT3 startScale =
	{
		baseSize.x * 1.5f,
		baseSize.y * 1.5f,
		baseSize.z,
	};
	XMFLOAT3 targetScale =
	{
		baseSize.x,
		baseSize.y,
		baseSize.z,
	};
	scale = LerpXMF3(startScale, targetScale, t);

	image.SetLocalTransform(
		Transform3D(
			image.GetLocalTransform().position,
			scale,
			DirectX::XMFLOAT3{ 0.0f, 0.0f, rotationZ }
		)
	);
}

//スタート表示更新関数
void CountUI::UpdateStartImage(UIImage& image)
{
	const float activeDuration = 90.0f;	//表示時間(フレーム数)
	const float moveDuration = 30.0f;	//移動時間(フレーム数)

	float t = Clamp01(m_mainTimer / moveDuration);
	t = EaseOutBack(t); //イージング適用

	if(m_mainTimer <= moveDuration)
	{
		float positionX = 0.0f;
		positionX = Lerpf(1920.0f * 0.5f, 0.0f, t); //X位置計算(画面外左端から中央へ移動)
		image.SetLocalTransform(
			Transform3D(
				DirectX::XMFLOAT3{ positionX, image.GetLocalTransform().position.y, image.GetLocalTransform().position.z },
				image.GetLocalTransform().scale,
				image.GetLocalTransform().rotation
			)
		);
	}

	if(m_mainTimer >= activeDuration)
	{
		//表示時間を超えたら非アクティブに設定
		image.SetActive(false);
		return;
	}
}

//フィニッシュ表示更新関数
void CountUI::UpdateFinishImage(UIImage& image)
{
	const float activeDuration = 90.0f; //表示時間(フレーム数)
	const float moveDuration = 30.0f;	//移動時間(フレーム数)

	float t = Clamp01(m_mainTimer / moveDuration);
	t = EaseOutBack(t); //イージング適用

	if (m_mainTimer <= moveDuration)
	{
		float positionY = 0.0f;
		positionY = Lerpf(1080.0f * 0.5f, 0.0f, t); //Y位置計算(画面外下端から中央へ移動)
		image.SetLocalTransform(
			Transform3D(
				DirectX::XMFLOAT3{ image.GetLocalTransform().position.x, positionY, image.GetLocalTransform().position.z },
				image.GetLocalTransform().scale,
				image.GetLocalTransform().rotation
			)
		);
	}
}