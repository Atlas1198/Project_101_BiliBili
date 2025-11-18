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
	m_inputInfo.t.trigger = Keyboard_IsKeyDownTrigger(KK_T);
	m_inputInfo.f.trigger = Keyboard_IsKeyDownTrigger(KK_F);
	m_inputInfo.g.trigger = Keyboard_IsKeyDownTrigger(KK_G);
	m_inputInfo.h.trigger = Keyboard_IsKeyDownTrigger(KK_H);
	m_inputInfo.i.trigger = Keyboard_IsKeyDownTrigger(KK_I);
	m_inputInfo.j.trigger = Keyboard_IsKeyDownTrigger(KK_J);
	m_inputInfo.k.trigger = Keyboard_IsKeyDownTrigger(KK_K);
	m_inputInfo.l.trigger = Keyboard_IsKeyDownTrigger(KK_L);
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
	m_inputInfo.t.down = Keyboard_IsKeyDown(KK_T);
	m_inputInfo.f.down = Keyboard_IsKeyDown(KK_F);
	m_inputInfo.g.down = Keyboard_IsKeyDown(KK_G);
	m_inputInfo.h.down = Keyboard_IsKeyDown(KK_H);
	m_inputInfo.i.down = Keyboard_IsKeyDown(KK_I);
	m_inputInfo.j.down = Keyboard_IsKeyDown(KK_J);
	m_inputInfo.k.down = Keyboard_IsKeyDown(KK_K);
	m_inputInfo.l.down = Keyboard_IsKeyDown(KK_L);
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
	m_inputInfo.t.up = Keyboard_IsKeyUp(KK_T);
	m_inputInfo.f.up = Keyboard_IsKeyUp(KK_F);
	m_inputInfo.g.up = Keyboard_IsKeyUp(KK_G);
	m_inputInfo.h.up = Keyboard_IsKeyUp(KK_H);
	m_inputInfo.i.up = Keyboard_IsKeyUp(KK_I);
	m_inputInfo.j.up = Keyboard_IsKeyUp(KK_J);
	m_inputInfo.k.up = Keyboard_IsKeyUp(KK_K);
	m_inputInfo.l.up = Keyboard_IsKeyUp(KK_L);
	m_inputInfo.up.up = !Keyboard_IsKeyUp(KK_UP);
	m_inputInfo.down.up = !Keyboard_IsKeyUp(KK_DOWN);
	m_inputInfo.left.up = !Keyboard_IsKeyUp(KK_LEFT);
	m_inputInfo.right.up = !Keyboard_IsKeyUp(KK_RIGHT);
	m_inputInfo.space.up = !Keyboard_IsKeyUp(KK_SPACE);
	m_inputInfo.enter.up = !Keyboard_IsKeyUp(KK_ENTER);
}