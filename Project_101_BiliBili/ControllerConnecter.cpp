#include "ControllerConnecter.h"
#include "InputManager.h"
#include "EventManager.h"

//初期化
void ControllerConnecter::Initialize()
{
}

//更新
void ControllerConnecter::Update(InputManager& inputManager)
{
	auto info = inputManager.GetInputInfo();

	for(int i = 0; i < 4; ++i)
	{
		//既に接続済みならスルー
		if (!m_connectionStatuses[i].isConnected)
		{
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
		{
			//ボタン入力がなければスルー
			if (!info->controller[i].anyButton.trigger || !info->controller[i].anyButton.down) continue;

			//UIの入力リアクションを呼び出し
			EventManager::GetInstance()->TriggerEvent<std::pair<int, InputInfo&>>(
				EventType::CONTROLLER_ICON_REACTION, {i, *info});
		}
	}
}

//終了
void ControllerConnecter::Finalize()
{
}
