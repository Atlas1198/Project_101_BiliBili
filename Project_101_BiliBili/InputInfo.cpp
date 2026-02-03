#include "InputInfo.h"
#include "InputManager.h"

//コントローラー振動セット
void ControllerInputInfo::SetVibration(float leftMotor, float rightMotor, int duration)
{
	pInputManager->SetControllerVibration(index, leftMotor, rightMotor, duration);
}

//コントローラー振動停止
void ControllerInputInfo::StopVibration()
{
	pInputManager->StopControllerVibration(index);
}
