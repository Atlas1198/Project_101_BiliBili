#pragma once

enum EventType
{
	ITEM_PICKUP, // int teamID
	TAKE_DAMAGE, // int teamID, float damage
	UPDATE_HP_UI, // int teamID, float newHP
	UPDATE_BULLET_UI, // int teamID, int newBulletCount
	GAME_OVER, // int winningTeamID
};