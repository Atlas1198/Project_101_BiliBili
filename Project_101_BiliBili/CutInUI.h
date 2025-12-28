#pragma once
#include "UIBase.h"
#include "UIimage.h"
#include "SharedStruct.h"

//カットインUIクラス
class CutInUI : public UIBase
{
public:	//公開関数
	enum class STATE
	{
		NONE = 0,	//なし
		STATE_IN,	//導入
		STATE_STAY,	//滞在
		STATE_OUT	//終了
	};
public:
	CutInUI(
		DirectX::XMFLOAT3 position,
		DirectX::XMFLOAT3 scale,
		DirectX::XMFLOAT3 rotation,
		UINT order,
		DIRECTION direction
	);
	~CutInUI();
	void InitializeOverride(
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;
	void FinalizeOverride() override;

	void CallCutIn(	//カットイン呼び出し関数
		int characterIndex	//キャラクターインデックス(0～3)
	);

private:
	UIImage* m_pCutInImage[4] = { nullptr };							//カットイン画像UIポインタ(キャラクター4人分)
	DIRECTION m_direction;												//カットインの方向
	STATE m_state[4] = { STATE::NONE };									//カットインの状態
	DirectX::XMFLOAT3 m_initialPosition[4] = { {0.0f, 0.0f, 0.0f} };	//カットインの初期位置

	int m_mainTimer = 0;					//メインタイマー
	int m_indexStartTime[4] = { 0 };		//インデックスタイマー(キャラクター4人分)

private:
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	) override;

	void Start(int characterIndex);
	void Stay(int characterIndex);
	void End(int characterIndex);
};