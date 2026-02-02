#pragma once
#include "SharedStruct.h"
#include "Controller.h"
#include "InputInfo.h"

//入力管理クラス
class InputManager
{
private:
	InputInfo m_inputInfo{};		//入力情報構造体
	Controller m_controller;	//コントローラー管理クラス

public:
	InputManager() {};	//コンストラクタ
	~InputManager() { StopAllControllerVibrations(); };	//デストラクタ
	void Initialize();	//初期化
	void Update();		//更新
	void Copy();		//キー情報コピー

	//ゲッター
	InputInfo* GetInputInfo();	//入力情報構造体取得

	void SetControllerVibration(int index, float leftMotor = 1.0f, float rightMotor = 1.0f, int duration = 5); //コントローラー振動セット
	void SetAllControllerVibrations(float leftMotor = 1.0f, float rightMotor = 1.0f, int duration = 5);		//全コントローラー振動セット
	void StopControllerVibration(int index); //コントローラー振動停止
	void StopAllControllerVibrations(); //全コントローラー振動停止

private:
	void UpdateTriggerKeyInfo();	//トリガー情報更新
	void UpdateDownKeyInfo();		//押下情報更新
	void UpdateUpKeyInfo();			//離上情報更新
};