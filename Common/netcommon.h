#pragma once
#include <cstdint>
#include "network.h"

enum class GameMsg : uint32_t
{
	Server_GetStatus,
	Server_GetPing,
	//Server_RespondDesignerRequest,
	//Server_ChangeParameter,

	Client_Accepted,
	Client_AssignID,
	Client_RegisterWithServer,
	Client_UnregisterWithServer,
	//Client_RequestDesigner,
	//Client_RequestChangeParameter,

	//Lobby_PlayerJoined,
	//Lobby_PlayerLeft,
	//Lobby_StartGame,

	Game_AddPlayer,
	Game_RemovePlayer,
	Game_UpdatePlayer,
};

struct Vec3
{
	float x;
	float y;
	float z;
};

struct PlayerDescription
{
	uint32_t uniqueID = 0;
	uint32_t ingameID = 0;
	//uint32_t nAvatarID = 0;

	Vec3 pos = {0.0f, 0.0f, 0.0f};
	//float radius = 0.0f;
};