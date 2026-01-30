#pragma once
#include <d3d12.h>
#include "d3dx12.h"
#include <vector>
#include <memory>
#include <utility>
#include "UIBase.h"
#include "UIImage.h"
#include "SharedStruct.h"
#include "RenderData.h"

// 前方宣言
class Renderer;
class TextureManager;
class MeshManager;

enum class FADE_STATE
{
	FADE_NONE = 0,	//フェードなし
	FADE_IN,		//フェードイン
	FADE_OUT,		//フェードアウト
	FADE_COMPLETE	//フェード完了
};

//UI管理クラス
class UIManagerBase
{
public:	//公開関数
	UIManagerBase(
		CameraInfo* cameraInfo,
		float screenWidth = 0.0f,
		float screenHeight = 0.0f
	) 
		: 
		m_pCameraInfo(cameraInfo),
		m_screenWidth(screenWidth),
		m_screenHeight(screenHeight)
	{};	//コンストラクタ

	~UIManagerBase();	//デストラクタ

	//メイン処理関数
	void Initialize(										//初期化
		TextureManager& textureManager,	//テクスチャ管理クラス
		MeshManager& meshManager		//メッシュ管理クラス
	);
	void Update();											//更新
	void SubmitDraws(Renderer& renderer);					//描画要求をシーンに提出
	void Finalize();										//終了

	//フェード関連関数
	void StartFadeIn(float duration, int delay = 0);	//フェードイン開始
	void StartFadeOut(float duration, int delay = 0);	//フェードアウト開始
	bool IsFading();				//フェード中かどうかを取得
	bool IsFadeEnd();				//フェード終了かどうかを取得

protected:
	virtual void InitializeOverride(							//初期化(派生クラスでオーバーライド)
		TextureManager& textureManager,
		MeshManager& meshManager
	) = 0;
	virtual void UpdateOverride() = 0;							//更新(派生クラスでオーバーライド)
	virtual void FinalizeOverride() = 0;						//終了(派生クラスでオーバーライド)

	void PrepareRenderInfo(	//オブジェクトの描画情報生成
		TextureManager& textureManager,	//テクスチャ管理クラスの参照
		MeshManager& meshManager		//メッシュ管理クラスの参照
	);

	void SubmitRenderInfo(	//描画情報をシーンに提出
		Renderer& renderer,							//シーンの参照
		std::vector<WorldRenderInfo>& info	//描画情報構造体
	);

protected:
	std::vector<std::unique_ptr<UIBase>> m_roots;		//ルートUIオブジェクト配列
	float m_screenWidth = 0.0f;	//画面幅
	float m_screenHeight = 0.0f;	//画面高さ

	UIImage* m_pFadeImage = nullptr;				//フェード画像UIポインタ
	float m_fadeDuration = 0.0f;					//フェード時間
	float m_fadeDelay = 0.0f;						//フェード遅延時間
	int m_fadeTimer = 0;							//フェードタイマー
	FADE_STATE m_fadeState = FADE_STATE::FADE_NONE; //フェード状態

	CameraInfo* m_pCameraInfo = nullptr;		//カメラ情報構造体参照

private:
	uint64_t m_fadeStartEventID = 0;	//フェード開始イベントID
	uint64_t m_fadeEndEventID = 0;		//フェード終了イベントID

private:
	void UpdateFade();	//フェード更新関数
};