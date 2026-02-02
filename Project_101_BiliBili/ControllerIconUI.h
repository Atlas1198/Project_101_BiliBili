#pragma once
#include "UIBase.h"
#include "UIImage.h"

struct InputInfo;

//コントローラーアイコンUIクラス
class ControllerIconUI : public UIBase
{
	public:	//公開関数
	ControllerIconUI(
		DirectX::XMFLOAT3 position = { 0,0,0 },
		DirectX::XMFLOAT3 scale = { 1,1,1 },
		DirectX::XMFLOAT3 rotation = { 0,0,0 },
		UINT order = 0,
		const std::wstring& textureFilePath = L"",
		BLEND_MODE blendMode = BLEND_MODE::BLEND_TRANSPARENT
	);
	~ControllerIconUI() = default;	//デストラクタ

	void ActivateReaction(const InputInfo& info);	//アクティブリアクション
protected:
	//仮想関数群
	void InitializeOverride(		//初期化(派生クラスでオーバーライド)
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;		//更新(派生クラスでオーバーライド)
	void FinalizeOverride() override;	//終了(派生クラスでオーバーライド)
	void PrepareRenderInfoOverride(	//オブジェクトの描画情報生成(派生クラスでオーバーライド)
		TextureManager& textureManager,
		MeshManager& meshManager
	) override {
	};

private:
	std::wstring m_textureFilePath;		// テクスチャファイルパス
	UIImage* m_pIconImage = nullptr;	// アイコン画像UIオブジェクト
	bool m_isActivated = false;			// アクティブリアクション中フラグ
	bool m_isReacting = false;			// リアクション中フラグ
	int m_reactionFrameCount = 0;		// リアクションフレームカウント
};