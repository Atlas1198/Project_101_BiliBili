#include "InputManager.h"
#include "Keyboard.h"

//初期化
void InputManager::Initialize()
{
	Keyboard_Initialize(); //キーボード初期化
	m_controller.Initialize();	//追加コントローラー初期化
}

//更新
void InputManager::Update()
{
	UpdateTriggerKeyInfo();	// Update Trigger Key Info
	UpdateDownKeyInfo();	// Update Down Key Info
	UpdateUpKeyInfo();		// Update Up Key Info

	m_controller.Update(m_inputInfo.controller);	// Update Controller Info

}

//キー情報コピー
void InputManager::Copy()
{
	keycopy(); //キーボードキー情報コピー
	m_controller.CopyState();	//コントローラー状態子コピー
}

//入力情報構造体取得
InputInfo* InputManager::GetInputInfo()
{
	return &m_inputInfo;
}


//トリガー情報更新
void InputManager::UpdateTriggerKeyInfo()
{
	m_inputInfo.key.w.trigger = Keyboard_IsKeyDownTrigger(KK_W);
	m_inputInfo.key.a.trigger = Keyboard_IsKeyDownTrigger(KK_A);
	m_inputInfo.key.s.trigger = Keyboard_IsKeyDownTrigger(KK_S);
	m_inputInfo.key.d.trigger = Keyboard_IsKeyDownTrigger(KK_D);
	m_inputInfo.key.p.trigger = Keyboard_IsKeyDownTrigger(KK_P);
	m_inputInfo.key.up.trigger = Keyboard_IsKeyDownTrigger(KK_UP);
	m_inputInfo.key.down.trigger = Keyboard_IsKeyDownTrigger(KK_DOWN);
	m_inputInfo.key.left.trigger = Keyboard_IsKeyDownTrigger(KK_LEFT);
	m_inputInfo.key.right.trigger = Keyboard_IsKeyDownTrigger(KK_RIGHT);
	m_inputInfo.key.space.trigger = Keyboard_IsKeyDownTrigger(KK_SPACE);
}

//押下情報更新
void InputManager::UpdateDownKeyInfo()
{
	m_inputInfo.key.w.down = Keyboard_IsKeyDown(KK_W);
	m_inputInfo.key.a.down = Keyboard_IsKeyDown(KK_A);
	m_inputInfo.key.s.down = Keyboard_IsKeyDown(KK_S);
	m_inputInfo.key.d.down = Keyboard_IsKeyDown(KK_D);
	m_inputInfo.key.p.down = Keyboard_IsKeyDown(KK_P);
	m_inputInfo.key.up.down = Keyboard_IsKeyDown(KK_UP);
	m_inputInfo.key.down.down = Keyboard_IsKeyDown(KK_DOWN);
	m_inputInfo.key.left.down = Keyboard_IsKeyDown(KK_LEFT);
	m_inputInfo.key.right.down = Keyboard_IsKeyDown(KK_RIGHT);
	m_inputInfo.key.space.down = Keyboard_IsKeyDown(KK_SPACE);
}

//離上情報更新
void InputManager::UpdateUpKeyInfo()
{
	m_inputInfo.key.w.up = Keyboard_IsKeyUp(KK_W);
	m_inputInfo.key.a.up = Keyboard_IsKeyUp(KK_A);
	m_inputInfo.key.s.up = Keyboard_IsKeyUp(KK_S);
	m_inputInfo.key.d.up = Keyboard_IsKeyUp(KK_D);
	m_inputInfo.key.p.up = Keyboard_IsKeyUp(KK_P);
	m_inputInfo.key.up.up = Keyboard_IsKeyUp(KK_UP);
	m_inputInfo.key.down.up = Keyboard_IsKeyUp(KK_DOWN);
	m_inputInfo.key.left.up = Keyboard_IsKeyUp(KK_LEFT);
	m_inputInfo.key.right.up = Keyboard_IsKeyUp(KK_RIGHT);
	m_inputInfo.key.space.up = Keyboard_IsKeyUp(KK_SPACE);
}