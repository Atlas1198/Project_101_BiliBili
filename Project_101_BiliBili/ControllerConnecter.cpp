#include "ControllerConnecter.h"
#include "InputManager.h"
#include "EventManager.h"
#include "SceneManager.h"

//初期化
void ControllerConnecter::Initialize()
{
	for (int i = 0; i < 4; ++i)
	{
		m_connectionStatuses[i].isConnected = false;
		m_connectionStatuses[i].controllerIndex = -1;
	}

	m_isAllConnected = false;
}

//更新
void ControllerConnecter::Update(InputManager& inputManager)
{
	//入力情報取得
	auto info = inputManager.GetInputInfo();

	//テスト用キーボード入力
	if (m_isAllConnected)
	{
		if (info->key.space.trigger)
		{
			//キャラクター選択シーンへの遷移イベント発行
			EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
				EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_CHARACTER);
		}
	}
	else
	{
		if (info->key.space.trigger)
		{//全コントローラー接続済みにする
			m_isAllConnected = true;
			for (int i = 0; i < 4; ++i)
			{
				m_connectionStatuses[i].isConnected = true;
				m_connectionStatuses[i].controllerIndex = i;
				//コントローラー接続イベント発行
				EventManager::GetInstance()->TriggerEvent<int>(
					EventType::CONTROLLER_CONNECTED, i);
			}
		}

		if(info->key.rightCtrl.trigger)
		{//未接続のコントローラーを1つ接続済みにする
			for (int i = 0; i < 4; ++i)
			{
				if (m_connectionStatuses[i].isConnected) continue;

				m_connectionStatuses[i].isConnected = true;
				m_connectionStatuses[i].controllerIndex = i;
				//コントローラー接続イベント発行
				EventManager::GetInstance()->TriggerEvent<int>(
					EventType::CONTROLLER_CONNECTED, i);
				break;
			}
		}
	}

	//コントローラー入力処理
	for (int i = 0; i < 4; ++i)
	{
		if (m_isAllConnected)
		{//全コントローラー接続済み
			if (info->controller[i].LSHOULDER.down && info->controller[i].RSHOULDER.down)
			{//いずれかのコントローラーでL+R同時押し検知
				//キャラクター選択シーンへの遷移イベント発行
				EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
					EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_CHARACTER);
			}
			else
			{//それ以外の入力処理
				//ボタン入力がなければスルー
				if (!info->controller[i].anyButton.trigger || !info->controller[i].anyButton.down) continue;

				//UIの入力リアクションを呼び出し
				EventManager::GetInstance()->TriggerEvent<std::pair<int, InputInfo&>>(
					EventType::CONTROLLER_ICON_REACTION, { i, *info });
			}
		}
		else 
		{//全コントローラー接続済みでない
			if (!m_connectionStatuses[i].isConnected)
			{//未接続のコントローラー
				//一度入力を検知したら接続済みにする
				if (info->controller[i].anyButton.trigger)
				{
					m_connectionStatuses[i].isConnected = true;
					m_connectionStatuses[i].controllerIndex = i;

					//コントローラー接続イベント発行
					EventManager::GetInstance()->TriggerEvent<int>(
						EventType::CONTROLLER_CONNECTED, i);
				}
			}
			else
			{//接続済みのコントローラー
				//ボタン入力がなければスルー
				if (!info->controller[i].anyButton.trigger || !info->controller[i].anyButton.down) continue;

				//UIの入力リアクションを呼び出し
				EventManager::GetInstance()->TriggerEvent<std::pair<int, InputInfo&>>(
					EventType::CONTROLLER_ICON_REACTION, { i, *info });
			}
		}
	}

	//全コントローラー接続済み判定更新
	bool allConnected = true;

	for (auto& state : m_connectionStatuses)
	{
		allConnected &= state.isConnected;
	}

	m_isAllConnected = allConnected;
}

//終了
void ControllerConnecter::Finalize()
{
}
