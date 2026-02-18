#pragma once
#include "ObjectManagerBase.h"
#include "Player.h"
#include "SharedStruct.h"
#include "BulletManager.h"
#include "EventType.h"
#include "PlayerShadow.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

//プレイヤー管理クラス
class PlayerManager : public ObjectManagerBase
{
public:
	//static constexpr int PLAYER_NUM = 2; // プレイヤーの数
	const wchar_t* modelFile = L"asset/fbx/Dragon 2.5_fbx.fbx";
	const wchar_t* texPath = L"asset/texture/player/Character_front.png";

	static constexpr DirectX::XMFLOAT3 spawnPoses[4] = {
	{-20.0f, -4.0f, 17.0f},
	{20.0f, -4.0f, 17.0f},
	{-20.0f, -4.0f, -7.0f},
	{20.0f, -4.0f, -7.0f}
	};

private:
	std::vector<Player*> m_pPlayer = std::vector<Player*>();	//プレイヤーオブジェクト配列
	std::vector<PlayerShadow *> m_pPlayerShadow = std::vector<PlayerShadow *>();	//プレイヤーの影オブジェクト配列
	std::vector<WorldRenderInfo> m_playerInfo[4];					//プレイヤー描画情報
	std::vector<WorldRenderInfo> m_playerTransformInfo[4];			//プレイヤー変身描画情報
	std::vector<WorldRenderInfo> m_shadowInfo;						//影描画情報
	float teamHP[2] = { 1.0f, 1.0f };							//チームの体力
	bool teamBBActive[2] = { false, false };					//チームのBBアクティブフラグ
	bool sentBBEnhanceEvent[2] = { false, false };			//チームのBB強化イベント送信フラグ

	std::vector<EventData> m_subscribedEvents;	//購読しているイベント配列

public:
	PlayerManager();			//コンストラクタ
	~PlayerManager();	//デストラクタ

	//メイン処理関数
	void InitializeOverride(	//初期化
		TextureManager& pTextureManager,	//テクスチャ管理クラスのポインタ
		MeshManager& pMeshManager,			//メッシュ管理クラスのポインタ
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) override;

	Player* AddPlayer(
		uint32_t id,						//ID
		InputManager *pInputManager,		//入力マネージャーのポインタ
		CollisionManager &collisionManager,	//衝突管理クラスの参照
		BulletManager *pBulletManager	//弾丸管理クラスの参照
	);
	void RemovePlayer(uint32_t id);

	void OnTakeDamage(int teamID, float damage); // ダメージを受けたときの処理
	void OnSetBB(int teamID, bool isActive); // BBをセットしたときの処理

	void UpdateOverride() override;		//更新
	void ResolveCollisionsOverride() override;	//衝突後処理
	void FinalizeOverride() override;	//終了

	//描画
	void SubmitDrawsOverride(Renderer& renderer) override;	//描画要求をシーンに提出

	//ゲッター
	std::vector<Player*>& GetPlayers(); // プレイヤーオブジェクト配列を取得

private:	//非公開関数
	void PrepareRenderInfo(	//プレイヤー描画情報生成
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;	
};