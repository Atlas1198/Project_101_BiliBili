#include "ControllerConnecter.h"
#include "InputManager.h"
#include "EventManager.h"
#include "SceneManager.h"
#include "AudioManager.h"

//初期化
void ControllerConnecter::Initialize()
{
	for (int i = 0; i < 4; ++i)
	{
		m_connectionStatuses[i].isConnected = false;
		m_connectionStatuses[i].controllerIndex = -1;
		m_backSceneInputTimer[i] = 0;
	}
	m_backSceneKeyInputTimer = 0;
	m_isAllConnected = false;
}

//更新
void ControllerConnecter::Update(SceneContext& sceneContext)
{
	//入力情報取得
	auto info = sceneContext.pInputInfo;

	const int BACK_SCENE_INPUT_DURATION = 90;

	//テスト用キーボード入力
	if (m_isAllConnected)
	{
		if (info->key.space.trigger)
		{
			//キャラクター選択シーンへの遷移イベント発行
			EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
				EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_STAGE);
			AudioManager::GetInstance()->PlaySE("CON_NEXT");
			AudioManager::GetInstance()->StopBGM();
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
				AudioManager::GetInstance()->PlaySE("CON_SET",1.5f);
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
				AudioManager::GetInstance()->PlaySE("CON_SET",1.6f);
				break;
			}
		}

		if (info->key.enter.down) m_backSceneKeyInputTimer++;
		else m_backSceneKeyInputTimer = 0;

		if (m_backSceneKeyInputTimer > BACK_SCENE_INPUT_DURATION) {
			EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
				EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_TITLE
			);
		}
	}

	//コントローラー入力処理
	for (int i = 0; i < 4; ++i)
	{
		auto& controller = info->controller[i];
		if (!controller.anyButton.trigger)
		{//入力検知なし
			continue;							//スルー
		}

		//以下、入力検知あり
		//全コントローラー接続済みかどうかで処理分岐
		if (m_isAllConnected)
		{//全コントローラー接続済み
			if (controller.LSHOULDER.down && controller.RSHOULDER.down)
			{//いずれかのコントローラーでL+R同時押し検知
				//キャラクター選択シーンへの遷移イベント発行
				info->SetAllControllerVibration(1.0f, 1.0f, 30);		//全コントローラー振動
				EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
					EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_STAGE);
				AudioManager::GetInstance()->PlaySE("CON_NEXT",1.6f);
				AudioManager::GetInstance()->StopBGM();
			}
			else 
			{//それ以外の入力処理
				//ボタン入力がなければスルー
				if (!controller.anyButton.trigger || !controller.anyButton.down) continue;

				controller.SetVibration(1.0f, 1.0f, 30); //入力検知時に振動させる
				//UIの入力リアクションを呼び出し
				EventManager::GetInstance()->TriggerEvent<std::pair<int, InputInfo>>(
					EventType::CONTROLLER_ICON_REACTION, { i, *info });
			}
		}
		else 
		{//全コントローラー接続済みでない
			if (!m_connectionStatuses[i].isConnected)
			{//未接続のコントローラー
				//一度入力を検知したら接続済みにする
				if (controller.anyButton.trigger)
				{
					m_connectionStatuses[i].isConnected = true;
					m_connectionStatuses[i].controllerIndex = i;

					controller.SetVibration(1.0f, 1.0f, 30); //入力検知時に振動させる
					//コントローラー接続イベント発行
					EventManager::GetInstance()->TriggerEvent<int>(
						EventType::CONTROLLER_CONNECTED, i);
					AudioManager::GetInstance()->PlaySE("CON_SET",1.6f);
				}
			}
			else
			{//接続済みのコントローラー
				//ボタン入力がなければスルー
				if (!controller.anyButton.trigger || !controller.anyButton.down) continue;

				controller.SetVibration(1.0f, 1.0f,30); //入力検知時に振動させる
				//UIの入力リアクションを呼び出し
				EventManager::GetInstance()->TriggerEvent<std::pair<int, InputInfo>>(
					EventType::CONTROLLER_ICON_REACTION, { i,*info });

				if (controller.A.down) m_backSceneInputTimer[i]++;
				else m_backSceneInputTimer[i] = 0;

				if (m_backSceneInputTimer[i] > BACK_SCENE_INPUT_DURATION){
					EventManager::GetInstance()->TriggerEvent<SCENE_TYPE>(
						EventType::CHANGE_SCENE, SCENE_TYPE::SCENE_TITLE
					);
				}
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
void ControllerConnecter::Finalize(SceneContext& sceneContext)
{
	//シーンコンテキストのプレイヤー情報を更新
	for(int i = 0; i < 4; ++i)
	{
		//コントローラーIDを設定(未接続の場合は-1)
		if (m_connectionStatuses[i].isConnected)
		{
			sceneContext.playersInfo[i].controllerID = m_connectionStatuses[i].controllerIndex;
		}
		else
		{
			sceneContext.playersInfo[i].controllerID = -1;
		}
	}
}
