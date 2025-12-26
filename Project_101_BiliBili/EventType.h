#pragma once

enum EventType
{
	ITEM_PICKUP, // int teamID
	TAKE_DAMAGE, // int teamID, float damage
	UPDATE_HP_UI, // int teamID, float newHP
	UPDATE_BULLET_UI, // int teamID, int newBulletCount
	GAME_OVER, // int winningTeamID
	ADD_EFFECT,
	CHANGE_SCENE, // int sceneID

	//プレイヤー設定関連イベント
	SET_PLAYER_CONTROLLER, // int playerIndex, int controllerID
	SET_PLAYER_CHARACTER, // int playerIndex, int characterID

	//コントローラー接続関連イベント
	CONTROLLER_CONNECTED, // int controllerIndex
	CONTROLLER_ICON_REACTION, // int controllerIndex, InputInfo inputInfo

	//キャラクター選択画面関連イベント
	CHARACTER_ICON_MOVE, // int playerIndex, DIRECTION direction
	SHOW_SELECTED_ICON, // int playerIndex
	HIDE_SELECTED_ICON, // int playerIndex
	GO_TO_GAME_SCENE, // void
};