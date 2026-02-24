#pragma once
#include "UIManagerBase.h"
#include "SharedStruct.h"
#include "Context.h"

// ステージ選択シーンUI管理クラス
class StageUIManager : public UIManagerBase
{
public:
	static constexpr int STAGE_NUM = static_cast<int>(STAGE_TYPE::STAGE_MAX);
public:
	StageUIManager(
		CameraInfo* cameraInfo,
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	)
		: UIManagerBase(cameraInfo, screenWidth, screenHeight)
	{};	//コンストラクタ
	~StageUIManager();	//デストラクタ

	void ChangeStageUI(STAGE_TYPE stageType);	//ステージUI変更関数
	void StartSelectAnimation();						//ステージ選択アニメーション開始関数

private:
	UIImage* m_pBackGround[STAGE_NUM] { nullptr };
	UIImage* m_pExplanation[STAGE_NUM] { nullptr };
	UIImage* m_pName[STAGE_NUM] { nullptr };

	UIImage* m_pHeader = nullptr;
	UIImage* m_pOperation = nullptr;
	UIImage* m_pArrow = nullptr;
	UIImage* m_pNameBack = nullptr;

	float m_explanationDestinationX = 0.0f;
	DirectX::XMFLOAT3 m_explanationOriginalPosition{ 0.0f, 0.0f, 0.0f };
	DirectX::XMFLOAT3 m_nameBackOriginalScale{ 1.0f, 1.0f, 1.0f };

	int m_timer = 0;
	int m_selectAnimationTimer = 0;
	bool m_isSelectAnimationStarted = false;

	//矢印アニメーション関連
	int m_arrowMoveTimer = 0;						//矢印移動タイマー
	DIRECTION m_arrowDirection = DIRECTION::NONE;	//矢印の方向

	STAGE_TYPE m_currentStageType = STAGE_TYPE::STAGE_GREEN;	//現在のステージタイプ

private:
	//メイン処理関数
	void InitializeOverride(							//初期化
		TextureManager& textureManager,
		MeshManager& meshManager
	) override;
	void UpdateOverride() override;						//更新
	void FinalizeOverride() override;					//終了
};