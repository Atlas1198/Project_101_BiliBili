#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include "ElectricityBB.h"
#include "LineBB.h"
#include "SharedStruct.h"

//前方宣言
class Renderer;
class GameUIManager;
class CollisionManager;

//BBクラス
class BB
{
public:
	static constexpr int PLAYER_NUM = 2; //プレイヤーの数
	//static constexpr float DAMAGE = 0.001f;	//ダメージ量
	inline static float DAMAGE = 0.001f;

public:
	BB(GameUIManager* pUIManager, CollisionManager* pCollisionManager);
	~BB();
	void Initialize();						//初期化
	void Update();							//更新
	void ResolveCollisions();				//衝突解決
	void Finalize();						//終了

	void ActivateBB();	//有効化
	void DisableBB();	//無効化

	//ゲッター
	LineBB* GetLineBB() const;			//ラインBBの取得
	ElectricityBB** GetElectricityBB();	//電気BB配列の取得
	bool IsActivated() const;			//発動中かどうか取得

	//セッター
	void SetPlayerPos(DirectX::XMFLOAT3 position[PLAYER_NUM]);	//プレイヤー位置の設定
	void SetTeamId(int id);										//チームIDの設定

private:
	LineBB* m_lineBB = nullptr;									//ラインBB
	ElectricityBB* m_electricityBB[PLAYER_NUM] = { nullptr };	//電気BB配列
	
	int m_teamId = -1;			//チームID
	bool m_activatedBB = false;	//発動中フラグ

	DirectX::XMFLOAT3 m_playerPos[PLAYER_NUM]; //プレイヤー位置配列

	GameUIManager* m_pUIManager = nullptr; //UIマネージャーへのポインタ
	CollisionManager* m_pCollisionManager = nullptr; //衝突マネージャーへのポインタ

private:
	void ControlElectricity();		//電流の操作
	DirectX::XMFLOAT3 GetClosestCollisionPos(	//プレイヤー座標から最も近い座標を取得
		DirectX::XMFLOAT3 position, 
		std::vector<DirectX::XMFLOAT3> collisionPointList
	);	
};