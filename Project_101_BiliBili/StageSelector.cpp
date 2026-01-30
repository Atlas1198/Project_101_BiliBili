#include "StageSelector.h"
#include "InputManager.h"
#include "EventManager.h"
#include "EventType.h"
#include <algorithm>

//初期化
void StageSelector::Initialize()
{
	m_cursor = 0;
	m_isSelected = false;
}

//更新
void StageSelector::Update(
	InputInfo& input,
	SceneContext& context
)
{
	if (m_isSelected) return;	//選択済みの場合は処理をスキップ

	m_previousCursor = m_cursor; //前回カーソル位置を保存

	bool left = false;		//左移動フラグ
	bool right = false;		//右移動フラグ
	bool select = false;	//決定フラグ

	const float DEAD_ZONE = 0.2f;	//スティックデッドゾーン

	//コントローラー入力取得
	for (int i = 0; i < CONTROLLERS_MAX; i++)
	{
		auto controller = input.controller[i];

		//決定(B)
		select |= controller.B.trigger;

		//カーソル操作
		//十字キー
		left |= controller.LEFT.trigger;
		right |= controller.RIGHT.trigger;
		//コントローラー
		float leftStickX = controller.leftStick.x;
		float leftStickPastX = controller.leftStickPast.x;
		if (fabs(leftStickX) < DEAD_ZONE || fabs(leftStickPastX) >= DEAD_ZONE) continue;	//トリガー入力なし
		left = leftStickX < 0.0f;
		right = leftStickX > 0.0f;
	}

	//キーボード入力
	select |= input.key.space.trigger;
	left |= input.key.left.trigger;
	right |= input.key.right.trigger;

	if (left) m_cursor--;
	if (right) m_cursor++;

	//カーソル位置をステージ種類内に限定
	if(m_cursor < 0)
	{
		m_cursor += static_cast<int>(STAGE_TYPE::STAGE_MAX);
	}
	m_cursor = m_cursor % static_cast<int>(STAGE_TYPE::STAGE_MAX);

	//カーソル位置が変化した場合
	if(m_cursor != m_previousCursor)
	{
		//ステージUI変更イベント発行
		EventManager::GetInstance()->TriggerEvent<STAGE_TYPE>(
			EventType::CHANGE_STAGE_UI,
			static_cast<STAGE_TYPE>(m_cursor)
		);

		//↓↓↓マップ切替時サウンド再生↓↓↓

	}

	//決定処理
	if (select)
	{
		context.stageType = static_cast<STAGE_TYPE>(m_cursor);
		m_isSelected = true;
	}
}

//終了
void StageSelector::Finalize()
{

}