#include "IconUI.h"

using namespace DirectX;

//コンストラクタ
IconUI::IconUI(
	XMFLOAT3 position, 
	XMFLOAT3 scale, 
	XMFLOAT3 rotation, 
	UINT order,
	const wchar_t* texturePath
)
	: UIBase(position, scale, rotation, order), m_texturePath(texturePath)
{
		//アイコン画像UIの作成
	m_pIconImage = AddChild<UIImage>(
		XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		XMFLOAT3{ 196.1f, 195.1f, 1.0f },
		XMFLOAT3{ 0.0f, 0.0f, 0.0f },
		m_order,
		m_texturePath,
		PSO_KEY_MULTIPLY
	);

	std::wstring characterTexturePaths[4] = {
		L"asset/texture/game_scene/UI_INGAME_Icon_1.png",
		L"asset/texture/game_scene/UI_INGAME_Icon_2.png",
		L"asset/texture/game_scene/UI_INGAME_Icon_3.png",
		L"asset/texture/game_scene/UI_INGAME_Icon_4.png"
	};

	const float scaleFactor = 0.6f;

	//キャラクター画像UIの作成
	for (int i = 0; i < 4; i++)
	{
		m_pCharacterImages[i] = AddChild<UIImage>(
			XMFLOAT3{ 0.0f, 0.0f, 0.0f },
			XMFLOAT3{ 256.0f * scaleFactor, 256.0f * scaleFactor, 1.0f },
			XMFLOAT3{ 0.0f, 0.0f, 0.0f },
			m_order + 1,
			characterTexturePaths[i],
			PSO_KEY_TRANSPARENT
		);
		m_pCharacterImages[i]->SetActive(false);
	}
}

//初期化
void IconUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

//更新
void IconUI::UpdateOverride()
{
}

//終了
void IconUI::FinalizeOverride()
{
}

//チームキャラクター設定
void IconUI::SetTeamCharacter(int p1, int p2)
{
	for(auto& characterImage : m_pCharacterImages)
	{
		characterImage->SetActive(false);
	}

	float parentPositionX = m_parent->GetLocalPosition().x;
	int sign = (parentPositionX >= 0.0f) ? 1 : -1;

	const XMFLOAT3 p1Position = {
		-35.0f * sign,
		-25.0f,
		0.0f
	};
	const XMFLOAT3 p2Position = {
		35.0f * sign,
		35.0f,
		0.0f };

	if(p1 >= 0 && p1 < 4)
	{
		m_pCharacterImages[p1]->SetActive(true);
		m_pCharacterImages[p1]->SetOrder(m_order + 2);
		m_pCharacterImages[p1]->SetLocalPosition(p1Position);
	}

	if (p2 >= 0 && p2 < 4)
	{
		m_pCharacterImages[p2]->SetActive(true);
		m_pCharacterImages[p2]->SetOrder(m_order + 1);
		m_pCharacterImages[p2]->SetLocalPosition(p2Position);
	}
}

//オブジェクトの描画情報生成
void IconUI::PrepareRenderInfoOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

