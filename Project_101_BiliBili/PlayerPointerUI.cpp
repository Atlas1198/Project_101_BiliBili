#include "PlayerPointerUI.h"

using namespace DirectX;

PlayerPointerUI::PlayerPointerUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order, PSOKey key)
	: UIBase(position, scale, rotation, order, key)
{
	//プレイヤーポインター画像テクスチャパス配列
	std::wstring playerPointerTexturePaths[4] =
	{
		L"asset/texture/game_scene/UI_INGAME_1p.png",
		L"asset/texture/game_scene/UI_INGAME_2p.png",
		L"asset/texture/game_scene/UI_INGAME_3p.png",
		L"asset/texture/game_scene/UI_INGAME_4p.png"
	};

	const float scaleFactor = 4.0f; //サイズ調整用の係数
	DirectX::XMFLOAT3 pointerScale = { POINTER_SCALE.x * scaleFactor, POINTER_SCALE.y * scaleFactor, 1.0f };


	for (int i = 0; i < 4; ++i)
	{
		m_pPointerImage[i] = AddChild<UIImage>(
			XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//位置
			pointerScale,						//スケール
			XMFLOAT3{ 0.0f, 0.0f, 0.0f },		//回転
			order + 1,							//描画順序（親UIより1つ大きい値）
			playerPointerTexturePaths[i]		//テクスチャパス
			);
		m_pPointerImage[i]->SetActive(false);	//最初は非表示
	}
}

void PlayerPointerUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
}

void PlayerPointerUI::UpdateOverride()
{
	for(int i = 0; i < 4; ++i)
	{
		if (m_pPointerImage[i]->IsActive())
		{
			const int moveTime = 60;
			auto position = m_pPointerImage[i]->GetLocalPosition();
			auto scale = m_pPointerImage[i]->GetLocalScale();
			auto targetPosition = m_playerPositions[i];
			auto targetScale = POINTER_SCALE;

			if (m_pointerTimers[i] < moveTime)
			{
				float t = static_cast<float>(m_pointerTimers[i]) / moveTime;
				float newPositionX = Lerpf(position.x, targetPosition.x, t);
				float newPositionY = Lerpf(position.y, targetPosition.y, t);
				float newScaleX = Lerpf(scale.x, targetScale.x, t);
				float newScaleY = Lerpf(scale.y, targetScale.y, t);
				m_pPointerImage[i]->SetLocalPosition(XMFLOAT3{ newPositionX, newPositionY, position.z });
				m_pPointerImage[i]->SetLocalScale(XMFLOAT3{ newScaleX, newScaleY, scale.z });
			}
			m_pointerTimers[i]++;	//タイマー更新
		}
	}
}

void PlayerPointerUI::FinalizeOverride()
{
}