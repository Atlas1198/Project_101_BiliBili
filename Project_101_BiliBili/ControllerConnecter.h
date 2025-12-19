#pragma once

class InputManager;

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
	void Update(InputManager& inputManager);	//更新
	void Finalize();							//終了

	bool IsAllConnected() const { return m_isAllConnected; }	//全コントローラー接続判定

private:
	ConnectionStatus m_connectionStatuses[4]{};	//コントローラー接続状態配列

	bool m_isAllConnected = false;	//全コントローラー接続フラグ
};