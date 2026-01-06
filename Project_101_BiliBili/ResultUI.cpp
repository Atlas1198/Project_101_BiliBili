#include "ResultUI.h"

//コンストラクタ
ResultUI::ResultUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order)
	: UIBase(position, scale, rotation, order)
{
	//背景画像UI生成
	m_pBackgroundImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//位置
		DirectX::XMFLOAT3{ 1920.0f, 1080.0f, 1.0f },		//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Back.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);
	m_pHeaderImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, 540.0f - 153.0f * 0.5f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 1920.0f, 153.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Header.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);
	m_pWinnerImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ -600.0f, 430.0f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 668.0f, 223.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Winner.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);

	float characterImageScale = 1300.0f; //キャラクター画像スケール
	m_pCharacterImage[0] = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, -0.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ characterImageScale, characterImageScale, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0,											//描画順序
		L"asset/texture/character_images/red_normal.png",					//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT				//ブレンドモード
	);
	m_pCharacterImage[1] = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, -0.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ characterImageScale, characterImageScale, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0,											//描画順序
		L"asset/texture/character_images/green_normal.png",					//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT				//ブレンドモード
	);
	m_pCharacterImage[2] = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, -0.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ characterImageScale, characterImageScale, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0,											//描画順序
		L"asset/texture/character_images/yellow_normal.png",					//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT				//ブレンドモード
	);
	m_pCharacterImage[3] = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, -0.0f, 0.0f },		//位置
		DirectX::XMFLOAT3{ characterImageScale, characterImageScale, 1.0f },	//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
		0,											//描画順序
		L"asset/texture/character_images/blue_normal.png",					//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT				//ブレンドモード
	);

	m_pFooterImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 0.0f, -540.0f + 296.0f * 0.5f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 1920.0f, 296.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Footer.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);
	m_pBlueImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ -960.0f + 700.0f, -540 + 331.5f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 1400.0f, 663.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Blue.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);
	m_pRedImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ -960.0f + 717.5f, -540.0f + 331.5f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 1435.0f, 663.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Red.png",	//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);
	m_pGoToTitleImage = AddChild<UIImage>(
		DirectX::XMFLOAT3{ 700.0f, -500.0f, 0.0f },			//位置
		DirectX::XMFLOAT3{ 424.0f, 55.0f, 1.0f },			//スケール
		DirectX::XMFLOAT3{ 0.0f, 0.0f, 0.0f },				//回転
		0,													//描画順序
		L"asset/texture/result_scene/UI_RESULT_Title.png",//テクスチャパス
		BLEND_MODE::BLEND_TRANSPARENT						//ブレンドモード
	);

	for(auto& characterImage : m_pCharacterImage)
	{
		characterImage->SetColor(DirectX::XMFLOAT4{ 1.2f, 1.2f, 1.2f, 1.0f }); 
	}

	//全ての子UIを非アクティブに設定
	for(auto& child : m_children)
	{
		child->SetActive(false); //全ての子UIを非アクティブに設定
	}
}

//初期化
void ResultUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//更新
void ResultUI::UpdateOverride()
{
	m_mainTimer++;	//メインタイマー更新
}

//終了
void ResultUI::FinalizeOverride()
{
}

void ResultUI::ShowResult(int winner, int character1ID, int character2ID)
{
	//全ての子UIをアクティブに設定
	for(auto& child : m_children)
	{
		child->SetActive(true);
	}

	//敗者チーム画像非アクティブ化
	if(winner == 0)
	{
		m_pRedImage->SetActive(false); //赤チーム画像非アクティブ
	}
	else if(winner == 1)
	{
		m_pBlueImage->SetActive(false); //青チーム画像非アクティブ
	}

	//キャラクター画像の位置設定
	m_pCharacterImage[character1ID]->SetLocalTransform(Transform3D{
		DirectX::XMFLOAT3{ 550.0f, -180.0f, 0.0f }, //位置
		m_pCharacterImage[character1ID]->GetLocalTransform().scale,
		m_pCharacterImage[character1ID]->GetLocalTransform().rotation
		});
	m_pCharacterImage[character2ID]->SetLocalTransform(Transform3D{
		DirectX::XMFLOAT3{ -50.0f, -180.0f, 0.0f }, //位置
		m_pCharacterImage[character2ID]->GetLocalTransform().scale,
		m_pCharacterImage[character2ID]->GetLocalTransform().rotation
		});

	//使用しないキャラクター画像非アクティブ化
	for(int i = 0; i < 4; i++)
	{
		if(i != character1ID && i != character2ID)
		{
			m_pCharacterImage[i]->SetActive(false);
		}
	}


	m_mainTimer = 0; //メインタイマーリセット
}

//オブジェクトの描画情報生成
void ResultUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	m_pBackgroundImage->PrepareRenderInfo(textureManager, meshManager);
}
