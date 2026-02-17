#pragma once
#include "UIBase.h"
#include "UIImage.h"

class PlayerPointerUI : public UIBase
{
public:
	static constexpr DirectX::XMFLOAT3 POINTER_SCALE = { 76.0f, 79.0f, 1.0f };	//プレイヤーポインタースケール
public:	//公開関数
	PlayerPointerUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0,
		PSOKey key = PSOKey{}
	);	//コンストラクタ
	~PlayerPointerUI() {};	//デストラクタ

	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了

	void SetPlayerActive(int playerIndex, bool isActive) {	//プレイヤーアクティブ設定
		if (playerIndex < 0 || playerIndex >= 4) return;	//インデックス範囲外チェック
		if (m_pPointerImage[playerIndex]) {
			m_pPointerImage[playerIndex]->SetActive(isActive);	//プレイヤーポインターのアクティブ設定
		}
	};	//プレイヤーアクティブ設定

	void SetPlayerPosition(int playerIndex, DirectX::XMFLOAT3 position) {
		if (playerIndex < 0 || playerIndex >= 4) return;	//インデックス範囲外チェック
		m_playerPositions[playerIndex] = position;			//プレイヤー位置設定
	};	//プレイヤー位置設定

protected:
	void PrepareRenderInfoOverride(						//描画情報生成
		TextureManager& textureManager,
		MeshManager& meshManager
	) override {
	};

private:
	UIImage* m_pPointerImage[4] = { nullptr };		//プレイヤーポインター画像UIオブジェクト配列
	DirectX::XMFLOAT3 m_playerPositions[4] = {};	//プレイヤー位置配列
	int m_pointerTimers[4] = { 0 };						//プレイヤーポインタースケールタイマー配列
};