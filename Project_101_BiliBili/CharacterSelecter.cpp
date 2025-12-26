#include "CharacterSelecter.h"
#include "InputManager.h"
#include "EventManager.h"

//初期化
void CharacterSelecter::Initialize()
{
	for (auto& state : m_states)
	{
		state.characterIndex = 0;
		state.isSelected = false;
	}
	m_isAllSelected = false;
}

//更新
void CharacterSelecter::Update(
	InputManager& inputManager,
	SceneContext& sceneContext
	)
{
	auto controllers = inputManager.GetInputInfo()->controller;

	if (m_isCalledGoToNextScene)
	{//次のシーンへ進む処理
		m_countToNextScene++;
		if (m_countToNextScene >= 90)
		{
			//ゲームシーンへの遷移イベント発行
			EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
				EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_GAME);
		}
	}
	else
	{
		//テスト用キーボード入力処理
		auto keyboard = inputManager.GetInputInfo()->key;
		if (keyboard.space.trigger)
		{
			if (m_isAllSelected)
			{
				m_isCalledGoToNextScene = true;
				m_countToNextScene = 0;
				EventManager::GetInstance()->TriggerEvent(
					EventType::GO_TO_GAME_SCENE);
			}
			else
			{
				for (int i = 0; i < 4; ++i)
				{
					auto& state = m_states[i];
					if (!state.isSelected)
					{
						state.characterIndex = 3 - i;
						EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(
							EventType::CHARACTER_ICON_MOVE, { i, state.characterIndex });
						state.isSelected = true;
						//選択済みアイコン表示イベント発行
						EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(
							EventType::SHOW_SELECTED_ICON, { i, state.characterIndex });
					}
				}
			}
		}
		//各プレイヤーの入力処理
		for (int i = 0; i < 4; ++i)
		{
			auto& state = m_states[i];	//選択状態

			int controllerID = sceneContext.playersInfo[i].controllerID;	//コントローラーID
			auto& controller = controllers[controllerID];					//コントローラー入力情報

			if (m_isAllSelected)
			{//全員選択済みの場合
				if (controller.B.trigger)
				{
					m_isCalledGoToNextScene = true;
					m_countToNextScene = 0;
					EventManager::GetInstance()->TriggerEvent(
						EventType::GO_TO_GAME_SCENE);
				}
			}

			if (state.isSelected)
			{//選択済みの場合
				if (controller.A.trigger)
				{//キャンセル入力
					state.isSelected = false;
					//選択済みアイコン非表示イベント発行
					EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(
						EventType::HIDE_SELECTED_ICON, { i, state.characterIndex });
				}
			}
			else if (!state.isSelected)
			{//未選択の場合
				if (controller.LEFT.trigger)
				{//左入力
					state.characterIndex = (std::max)(state.characterIndex - 1, 0);
					EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(
						EventType::CHARACTER_ICON_MOVE, { i, state.characterIndex });
				}
				else if (controller.RIGHT.trigger)
				{//右入力
					state.characterIndex = (std::min)(state.characterIndex + 1, 3);
					EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(
						EventType::CHARACTER_ICON_MOVE, { i, state.characterIndex });
				}
				else if (controller.B.trigger)
				{//決定入力
					bool selectedByOther = false;	//他のプレイヤーが選択済みかどうか

					//他のプレイヤーの選択状況確認
					for(auto& otherState : m_states)
					{
						if (&otherState != &state && otherState.isSelected &&
							otherState.characterIndex == state.characterIndex)
						{//他のプレイヤーが同じキャラクターを選択している場合は選択不可
							selectedByOther = true;
						}
					}

					//選択可能なら選択状態に変更
					if (!selectedByOther)
					{
						state.isSelected = true;
						//選択済みアイコン表示イベント発行
						EventManager::GetInstance()->TriggerEvent<std::pair<int, int>>(
							EventType::SHOW_SELECTED_ICON, { i, state.characterIndex });
					}
				}
			}
		}
	}

	//全員選択済みチェック
	m_isAllSelected = true;
	for (const auto& state : m_states)
	{
		if (!state.isSelected)
		{
			m_isAllSelected = false;
			break;
		}
	}
}

//終了
void CharacterSelecter::Finalize(SceneContext& sceneContext)
{
	for(int i = 0; i < 4; ++i)
	{
		sceneContext.playersInfo[i].characterID = m_states[i].characterIndex;
	}
}