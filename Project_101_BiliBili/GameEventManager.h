#pragma once
#include "EventType.h"
#include "FrameTimer.h"
#include <random>
#include <vector>
#include "EventManager.h"

template<typename T>
T RandomElement(const std::vector<T> &collection)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dist(0, collection.size() - 1);
	return collection[dist(gen)];
}



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

	void Update()
	{
		if (!started) return;

		if (currentEventIndex != -1)
		{
			if (timer.Peek() >= eventDuration)
			{
				StopCurrentEvent();
				timer.Mark();
			}
		}
		else
		{
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
		}
	}

private:
	void TriggerRandomEvent()
	{
		std::random_device rd;
		std::mt19937 gen(rd());
		std::uniform_int_distribution<> dist(0, eventList.size() - 1);
		int index = dist(gen);

		EventType event = eventList[index];
		currentEventIndex = index;

		EventManager::GetInstance()->TriggerEvent(event);
		EventManager::GetInstance()->TriggerEvent<EventType>(SHOW_ANOUNCE_UI, event);
	}

	void StopCurrentEvent()
	{
		if (currentEventIndex != -1)
		{
			EventManager::GetInstance()->TriggerEvent(eventList[currentEventIndex]);
			currentEventIndex = -1;
		}
	}

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