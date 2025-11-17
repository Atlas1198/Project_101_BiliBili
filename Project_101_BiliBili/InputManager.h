#pragma once
#include "SharedStruct.h"

//キー入力状態構造体
struct KeyInputState
{
	bool trigger = false;
	bool down = false;
	bool up = false;
};

//入力情報構造体
struct InputInfo
{
	KeyInputState w;
	KeyInputState a;
	KeyInputState s;
	KeyInputState d;
	KeyInputState up;
	KeyInputState down;
	KeyInputState left;
	KeyInputState right;
	KeyInputState space;
	KeyInputState enter;
};

//入力管理クラス
class InputManager
{
private:
	InputInfo m_inputInfo{};			//入力情報構造体

public:
	InputManager() {};	//コンストラクタ
	~InputManager() {};	//デストラクタ
	void Initialize();	//初期化
	void Update();		//更新
	void keyCopy();		//キー情報コピー

	//ゲッター
	InputInfo* GetInputInfo();	//入力情報構造体取得

private:
	void UpdateTriggerKeyInfo();	//トリガー情報更新
	void UpdateDownKeyInfo();		//押下情報更新
	void UpdateUpKeyInfo();			//離上情報更新
};