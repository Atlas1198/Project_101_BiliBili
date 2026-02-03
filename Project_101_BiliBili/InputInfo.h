#pragma once
#include "SharedStruct.h"

class InputManager; //前方宣言

static constexpr int CONTROLLERS_MAX = 4;

//キー入力状態構造体
struct InputState
{
	bool trigger = false;
	bool down = false;
	bool up = false;
};

struct KeyInputInfo
{
	InputState w;
	InputState a;
	InputState s;
	InputState d;
	InputState t;
	InputState f;
	InputState g;
	InputState h;
	InputState i;
	InputState j;
	InputState k;
	InputState l;
	InputState p;
	InputState z;
	InputState c;
	InputState n;
	InputState q;
	InputState e;
	InputState up;
	InputState down;
	InputState left;
	InputState right;
	InputState space;
	InputState enter;
	InputState rightCtrl;
	InputState one;
	InputState two;

	InputState any;
};

//コントローラー入力情報構造体
struct ControllerInputInfo
{
	InputState A;			//Aボタンor〇ボタン
	InputState B;			//Bボタンor×ボタン
	InputState X;			//Xボタンor△ボタン
	InputState Y;			//Yボタンor□ボタン
	InputState START;		//STARTボタン
	InputState BACK;		//BACKボタン
	InputState LSHOULDER;	//左肩ボタン
	InputState RSHOULDER;	//右肩ボタン
	InputState LTHUMB;		//左スティックの押下
	InputState RTHUMB;		//右スティックの押下
	InputState UP;			//上ボタン
	InputState DOWN;		//下ボタン
	InputState LEFT;		//左ボタン
	InputState RIGHT;		//右ボタン
	InputState anyButton;	//任意のボタン

	DirectX::XMFLOAT2 leftStick;	//Left stick(normalized)
	DirectX::XMFLOAT2 rightStick;	//Right stick(normalized)
	DirectX::XMFLOAT2 leftStickPast;	//Left stick(normalized)
	DirectX::XMFLOAT2 rightStickPast;	//Right stick(normalized)

	int index = -1; //コントローラーインデックス
	InputManager* pInputManager = nullptr; //入力マネージャーのポインタ
	void SetIndex(int idx) { index = idx; }
	void SetInputManager(InputManager* inputManager) { pInputManager = inputManager; }
	void SetVibration(float leftMotor = 1.0f, float rightMotor = 1.0f, int duration = 5);
	void StopVibration();
};

//入力情報構造体
struct InputInfo
{
	KeyInputInfo key;
	ControllerInputInfo controller[CONTROLLERS_MAX];

	void SetAllControllerVibration(float leftMotor = 1.0f, float rightMotor = 1.0f, int duration = 5)
	{
		for (int i = 0; i < CONTROLLERS_MAX; i++)
		{
			controller[i].SetVibration(leftMotor, rightMotor, duration);
		}
	}

	void StopAllControllerVibrations()
	{
		for (int i = 0; i < CONTROLLERS_MAX; i++)
		{
			controller[i].StopVibration();
		}
	}
};
