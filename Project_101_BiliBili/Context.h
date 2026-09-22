#pragma once
#include "InputInfo.h"

//プレイヤー情報構造体
struct PlayerInfo
{
	int controllerID = -1;	//コントローラーID
	int characterID = -1;	//キャラクターID
};

//ステージ種別列挙体
enum class STAGE_TYPE
{
	STAGE_GREEN,
	STAGE_RED,
	STAGE_BLUE,
	STAGE_TWO,
	STAGE_MAX,
	STAGE_NONE
};

//シーンコンテキスト構造体
struct SceneContext
{
	InputInfo* pInputInfo{};						//入力情報
	PlayerInfo playersInfo[4]{};					//プレイヤー情報配列
	STAGE_TYPE stageType = STAGE_TYPE::STAGE_NONE;	//選択ステージ種別
};
