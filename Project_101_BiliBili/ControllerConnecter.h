#pragma once
#include "SharedStruct.h"
#include "Context.h"

class InputManager;

//コントローラー接続状態構造体
struct ConnectionStatus
{
	bool isConnected;		//接続状態
	int controllerIndex;	//コントローラーインデックス
};

//コントローラー接続クラス
class ControllerConnecter
{
public:
	ControllerConnecter() = default;
	~ControllerConnecter() = default;

	void Initialize();							//初期化
	void Update(SceneContext& sceneContext);	//更新
	void Finalize(SceneContext& sceneContext);	//終了

	bool IsAllConnected() const { return m_isAllConnected; }	//全コントローラー接続判定

private:
	ConnectionStatus m_connectionStatuses[4]{};	//コントローラー接続状態配列

	int m_backSceneKeyInputTimer = 0;
	int m_backSceneInputTimer[4] = {0};	//前のシーンに戻るための長押し検知タイマー

	bool m_isAllConnected = false;	//全コントローラー接続フラグ
};