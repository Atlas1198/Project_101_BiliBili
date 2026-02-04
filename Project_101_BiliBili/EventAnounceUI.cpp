#include "EventAnounceUI.h"

using namespace DirectX;

//コンストラクタ
EventAnounceUI::EventAnounceUI(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 scale, DirectX::XMFLOAT3 rotation, UINT order, BLEND_MODE blendMode)
	:UIBase(position, scale, rotation, order, blendMode)
{
}

//初期化
void EventAnounceUI::InitializeOverride(TextureManager& textureManager, MeshManager& meshManager)
{
	const XMFLOAT3 POSITION = { 0.0f, 0.0f, 0.0f };
	const XMFLOAT3 SCALE = { 908.0f, 128.0f, 1.0f };
	const XMFLOAT3 ROTATION = { 0.0f, 0.0f, 0.0f };

	//BB画像UIの作成
	m_pBB = AddChild<UIImage>(
		POSITION,
		SCALE,
		ROTATION,
		m_order,
		L"asset/texture/game_scene/UI_INGAME_Announce_bilibili.png"
	);
	//速度上昇画像UIの作成
	m_pSpeed = AddChild<UIImage>(
		POSITION,
		SCALE,
		ROTATION,
		m_order,
		L"asset/texture/game_scene/UI_INGAME_Announce_speed.png"
	);
	//回復画像UIの作成
	m_pRecover = AddChild<UIImage>(
		POSITION,
		SCALE,
		ROTATION,
		m_order,
		L"asset/texture/game_scene/UI_INGAME_Announce_tama.png"
	);
	m_pItemSpawn = AddChild<UIImage>(
		POSITION,
		SCALE,
		ROTATION,
		m_order,
		L"asset/texture/game_scene/UI_INGAME_Announce_henshin.png"
	);
	for(auto& child : m_children)
	{
		child->SetActive(false);
	}
	m_timer = 0;
}

//更新
void EventAnounceUI::UpdateOverride()
{
	m_timer++;

	const int DISPLAY_TIME = 180; //表示時間(フレーム数)

	for (auto& child : m_children)
	{
		if(child->IsActive())
		{

			const int BLINK_COUNT = 3;
			auto color = child->GetColor();
			if (m_timer <= DISPLAY_TIME)
			{//表示時間内は点滅処理
				if (m_blinkCount < BLINK_COUNT)
				{
					const float ALPHA_DECREMENT = 0.06f;
					if (color.w == 1.0f)
					{
						//点滅音再生
					}
					color.w -= ALPHA_DECREMENT;
					if (color.w < 0.0f)
					{
						color.w = 1.0f;
						m_blinkCount++;
					}
					child->SetColor(color);
				}
				else
				{
					//点滅終了後は不透明に戻す
					auto color = child->GetColor();
					color.w = 1.0f;
					child->SetColor(color);
				}
			}
			else
			{//表示時間経過後はフェードアウト
				const float ALPHA_INCREMENT = 0.03f;
				color.w -= ALPHA_INCREMENT;
				child->SetColor(color);
				if (color.w < 0.0f)
				{
					color.w = 1.0f;
					child->SetColor(color);
					child->SetActive(false);
				}
			}
		}
	}
}

//終了
void EventAnounceUI::FinalizeOverride()
{
}

//アナウンス表示関数
void EventAnounceUI::ShowAnounce(EventType type)
{
	for(auto& child : m_children)
	{
		child->SetActive(false);
	}

	if (type == EVENT_BULLET_SPEED)
	{
		m_pSpeed->SetActive(true);
	}
	else if (type == EVENT_BULLET_RECOVERY)
	{
		m_pRecover->SetActive(true);
	}
	else if(type == EVENT_BB_ENHANCE)
	{
		m_pBB->SetActive(true);
	}
	else if (type == EVENT_ITEM_SPAWN)
	{
		m_pItemSpawn->SetActive(true);
	}

	m_timer = 0;
	m_blinkCount = 0;
}
