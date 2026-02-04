#pragma once

enum EventType
{
	NONE = 0,
	//ゲームプレイ関連イベント
	ITEM_PICKUP, // int teamID
	TAKE_DAMAGE, // int teamID, float damage
	GAME_OVER, // int winningTeamID
	ADD_EFFECT,
	CHANGE_SCENE, // int sceneID
	SET_BB, // int teamID, bool isActive
	DAMAGE_ANIMATION, // uint32_t playerID

	//プレイヤー設定関連イベント
	SET_PLAYER_CONTROLLER, // int playerIndex, int controllerID
	SET_PLAYER_CHARACTER, // int playerIndex, int characterID

	//コントローラー接続関連イベント
	CONTROLLER_CONNECTED, // int controllerIndex
	CONTROLLER_ICON_REACTION, // int controllerIndex, InputInfo inputInfo

	//ステージ選択画面関連イベント
	CHANGE_STAGE_UI, // STAGE_TYPE stageType

	//キャラクター選択画面関連イベント
	CHARACTER_ICON_MOVE, // int playerIndex, DIRECTION direction
	SHOW_SELECTED_ICON, // int playerIndex
	HIDE_SELECTED_ICON, // int playerIndex
	GO_TO_GAME_SCENE, // void

	//ゲームシーンUI関連イベント
	UPDATE_HP_UI, // int teamID, float newHP
	UPDATE_BULLET_UI, // int teamID, int newBulletCount
	BB_CUT_IN, // int teamID
	SHOW_COUNT_UI, // int count
	SHOW_START_UI, // void
	SHOW_FINISH_UI, // void
	HIDE_COUNT_UI, // void
	SHOW_RESULT_UI, // void
	SET_PLAYER_CHASING_UI_POSITION, // int teamID, XMFLOAT3 position1, XMFLOAT3 position2
	SET_BULLET_UI_ACTIVE, // int teamID, bool isActive
	INACTIVATE_PLAYER_POINTER_IMAGES, // void
	SHOW_ANOUNCE_UI, // EventType eventType

	//ゲーム内イベント(UI表示と併用)
	EVENT_BULLET_SPEED,		// void
	EVENT_BULLET_RECOVERY,	// void
	EVENT_BB_ENHANCE,		// void
	EVENT_ITEM_SPAWN,		// void
	//フェード関連イベント
	START_FADE_IN, // float duration
	START_FADE_OUT, // float duration
};

//イベントデータ構造体
struct EventData
{
	EventType type = EventType::NONE;
	uint64_t id = 0;
};

//イベントデータリストから特定のイベントデータを検索するヘルパー関数
inline static EventData FindEventData(
	const std::vector<EventData>& eventDataList,
	EventType type
)
{
	for (const auto& eventData : eventDataList)
	{
		if (eventData.type == type)
		{
			return eventData;
		}
	}

	return EventData{ EventType::NONE, 0 };
}