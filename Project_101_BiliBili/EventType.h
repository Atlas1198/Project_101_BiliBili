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
	CONTROLLER_CONNECTED, // int controllerIndex
	CONTROLLER_ICON_REACTION, // int controllerIndex, InputInfo inputInfo
};