#pragma once
#include "SceneBase.h"
#include "CharacterSelecter.h"
#include "CharacterUIManager.h"

//前方宣言
class InputManager;
class TextureManager;
class MeshManager;

//キャラクターシーンクラス
class CharacterScene : public SceneBase
{
public:	//公開関数
	CharacterScene(float window_width, float window_height);	//コンストラクタ
	~CharacterScene();											//デストラクタ

private:
	//メンバ変数
	CharacterSelecter* m_pCharacterSelecter = nullptr;		//キャラクター選択クラス
	CharacterUIManager* m_pCharacterUIManager = nullptr;	//キャラクターUI管理クラス

private:
	//メイン処理関数
	void InitializeOverride(									//初期化
		InputManager* pInputManager,		//入力管理クラスのポインタ
		TextureManager& pTextureManager,	//テクスチャ管理クラスの参照
		MeshManager& pMeshManager			//メッシュ管理クラスの参照
	) override;
	void UpdateOverride() override;						//更新
	void ResolveCollisions() override {};				//衝突後処理
	void DrawOverride(Renderer& pRenderer) override;	//描画
	void FinalizeOverride() override;					//終了
};