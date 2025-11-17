#include "InputManager.h"
#include "Keyboard.h"

//初期化
void InputManager::Initialize()
{
	Keyboard_Initialize(); //キーボード初期化
}

//更新
void InputManager::Update()
{
	UpdateTriggerKeyInfo();	//トリガー情報更新
	UpdateDownKeyInfo();	//押下情報更新
}

//キー情報コピー
void InputManager::keyCopy()
{
	keycopy(); //キーボードキー情報コピー
}

//入力情報構造体取得
InputInfo* InputManager::GetInputInfo()
{
	return &m_inputInfo;
}

//トリガー情報更新
void InputManager::UpdateTriggerKeyInfo()
{
	m_inputInfo.w.trigger = Keyboard_IsKeyDownTrigger(KK_W);
	m_inputInfo.a.trigger = Keyboard_IsKeyDownTrigger(KK_A);
	m_inputInfo.s.trigger = Keyboard_IsKeyDownTrigger(KK_S);
	m_inputInfo.d.trigger = Keyboard_IsKeyDownTrigger(KK_D);
	m_inputInfo.up.trigger = Keyboard_IsKeyDownTrigger(KK_UP);
	m_inputInfo.down.trigger = Keyboard_IsKeyDownTrigger(KK_DOWN);
	m_inputInfo.left.trigger = Keyboard_IsKeyDownTrigger(KK_LEFT);
	m_inputInfo.right.trigger = Keyboard_IsKeyDownTrigger(KK_RIGHT);
	m_inputInfo.space.trigger = Keyboard_IsKeyDownTrigger(KK_SPACE);
	m_inputInfo.enter.trigger = Keyboard_IsKeyDownTrigger(KK_ENTER);
}

//押下情報更新
void InputManager::UpdateDownKeyInfo()
{
	m_inputInfo.w.down = Keyboard_IsKeyDown(KK_W);
	m_inputInfo.a.down = Keyboard_IsKeyDown(KK_A);
	m_inputInfo.s.down = Keyboard_IsKeyDown(KK_S);
	m_inputInfo.d.down = Keyboard_IsKeyDown(KK_D);
	m_inputInfo.up.down = Keyboard_IsKeyDown(KK_UP);
	m_inputInfo.down.down = Keyboard_IsKeyDown(KK_DOWN);
	m_inputInfo.left.down = Keyboard_IsKeyDown(KK_LEFT);
	m_inputInfo.right.down = Keyboard_IsKeyDown(KK_RIGHT);
	m_inputInfo.space.down = Keyboard_IsKeyDown(KK_SPACE);
	m_inputInfo.enter.down = Keyboard_IsKeyDown(KK_ENTER);
}

//離上情報更新
void InputManager::UpdateUpKeyInfo()
{
	m_inputInfo.w.up = !Keyboard_IsKeyUp(KK_W);
	m_inputInfo.a.up = !Keyboard_IsKeyUp(KK_A);
	m_inputInfo.s.up = !Keyboard_IsKeyUp(KK_S);
	m_inputInfo.d.up = !Keyboard_IsKeyUp(KK_D);
	m_inputInfo.up.up = !Keyboard_IsKeyUp(KK_UP);
	m_inputInfo.down.up = !Keyboard_IsKeyUp(KK_DOWN);
	m_inputInfo.left.up = !Keyboard_IsKeyUp(KK_LEFT);
	m_inputInfo.right.up = !Keyboard_IsKeyUp(KK_RIGHT);
	m_inputInfo.space.up = !Keyboard_IsKeyUp(KK_SPACE);
	m_inputInfo.enter.up = !Keyboard_IsKeyUp(KK_ENTER);
}