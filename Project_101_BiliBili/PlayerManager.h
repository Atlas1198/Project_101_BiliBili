#pragma once
#include "GameObjectManager.h"
#include "Player.h"
#include "SharedStruct.h"

//前方宣言
class Renderer;
class InputManager;
class TextureManager;
class MeshManager;

//プレイヤー管理クラス
class PlayerManager : public GameObjectManager
{
public:
	//static constexpr int PLAYER_NUM = 2; // プレイヤーの数
	const wchar_t* modelFile = L"asset/fbx/Dragon 2.5_fbx.fbx";
	const wchar_t* texPath = L"asset/texture/white.png";

private:
	std::vector<Player*> m_pPlayer = std::vector<Player*>();	//プレイヤーオブジェクト配列
	std::vector<RenderData::RenderInfo> m_playerInfo;					//プレイヤー描画情報
	InputManager *m_pInputManager;

public:
	PlayerManager(){};			//コンストラクタ
	~PlayerManager();	//デストラクタ

	//メイン処理関数
	void Initialize(	//初期化
		InputManager* pInputManager,		//入力マネージャーのポインタ
		TextureManager& pTextureManager,	//テクスチャ管理クラスのポインタ
		MeshManager& pMeshManager,			//メッシュ管理クラスのポインタ
		CollisionManager& collisionManager	//衝突管理クラスの参照
	) override;

	void AddPlayer(
		uint32_t id,						//ID
		InputManager *pInputManager,		//入力マネージャーのポインタ
		CollisionManager &collisionManager	//衝突管理クラスの参照
	);
	void RemovePlayer(uint32_t id);

	void Update() override;		//更新
	void ResolveCollisions() override;	//衝突後処理

	//描画
	void SubmitDraws(Renderer& renderer);	//描画要求をシーンに提出

	//ゲッター
	Player* GetPlayer() const; // プレイヤーオブジェクトを取得

private:
	void PrepareRenderInfo(	//プレイヤー描画情報生成
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;	
};