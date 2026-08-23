#pragma once
#include "EventType.h"
#include "FrameTimer.h"
#include <random>
#include <vector>
#include "EventManager.h"
#include "AudioManager.h"

class GameEventManager
{
public:
	GameEventManager() { }

	void Start()
	{
		timer.Mark();
		currentEventIndex = -1;
		started = true;
	}

	void Stop()
	{
		started = false;
	}

	void Update()
	{
		if (!started) return;

		/*if (currentEventIndex != -1)
		{
			if (timer.Peek() >= eventDuration)
			{
				StopCurrentEvent();
				timer.Mark();
			}
		}
		else
		{*/
			if (timer.Peek() >= eventInterval)
			{
				if (skipsLeft > 0)
				{
					skipsLeft--;
				}
				else
				{
					TriggerRandomEvent();
				}
				timer.Mark();
			}
		//}
	}

private:
	void TriggerRandomEvent()
	{
		if (eventList.empty())
		{
			OutputDebugStringA("[GameEventManager] Event trigger skipped because the event list is empty\n");
			currentEventIndex = -1;
			return;
		}

		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<size_t> dist(0, eventList.size() - 1);
		const size_t index = dist(gen);

		EventType event = eventList[index];
		currentEventIndex = static_cast<int>(index);

		EventManager::GetInstance()->TriggerEvent(event);
		EventManager::GetInstance()->TriggerEvent<EventType>(SHOW_ANOUNCE_UI, event);
		AudioManager::GetInstance()->PlaySE("ANNOUNCE_ALL");
	}

	/*void StopCurrentEvent()
	{
		if (currentEventIndex != -1)
		{
			EventManager::GetInstance()->TriggerEvent(eventList[currentEventIndex]);
			currentEventIndex = -1;
		}
	}*/

private:
	float eventInterval = 20.0f; // イベント発生間隔（秒
	float eventDuration = 5.0f; // イベント持続時間（秒）
	FrameTimer timer;
	const std::vector<EventType> eventList = {
		EVENT_BULLET_SPEED,
		EVENT_BULLET_RECOVERY
	};
	int currentEventIndex = -1;
	bool started = false;
	int skipsLeft = 2;
};
