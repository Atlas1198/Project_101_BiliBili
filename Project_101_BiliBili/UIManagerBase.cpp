#include "UIManagerBase.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "MeshManager.h"
#include "EventManager.h"
#include "EventType.h"
#include <algorithm>

using namespace DirectX;

//デストラクタ
UIManagerBase::~UIManagerBase()
{
}

//初期化
void UIManagerBase::Initialize(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	InitializeOverride(textureManager, meshManager);

	m_pFadeImage = new UIImage(
		{ 0.0f, 0.0f, 0.0f },	//位置
		{ m_screenWidth, m_screenHeight, 1.0f },//スケール
		{ 0.0f, 0.0f, 0.0f },	//回転
		1000,					//描画順序
		L"asset/texture/white.png",
		BLEND_MODE::BLEND_TRANSPARENT
	);
	m_pFadeImage->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f }); //初期透明
	m_roots.push_back(std::unique_ptr<UIBase>(m_pFadeImage));

	PrepareRenderInfo(textureManager, meshManager);

	using args = float;
	m_fadeStartEventID = EventManager::GetInstance()->Subscribe<args>(
		EventType::START_FADE_IN,
		[this](std::shared_ptr<args> data){
			StartFadeIn(*data);
		}
	);

	m_fadeEndEventID = EventManager::GetInstance()->Subscribe<args>(
		EventType::START_FADE_OUT,
		[this](std::shared_ptr<args> data){
			StartFadeOut(*data);
		}
	);

	//フェード関連の初期化
	m_isFading = false;
	m_isFadeEnd = false;
}

//更新
void UIManagerBase::Update()
{
	UpdateOverride();

	for(auto& root : m_roots) 
	{
		root->Update();
	}

	UpdateFade();
}

//描画要求をシーンに提出
void UIManagerBase::SubmitDraws(Renderer& renderer)
{
	//描画順の更新(昇順)
	std::sort(
		m_roots.begin(),
		m_roots.end(),
		[](const std::unique_ptr<UIBase>& a, const std::unique_ptr<UIBase>& b) {
			return a->GetOrder() < b->GetOrder();
		}
	);

	//ワールド変換情報の更新
	Transform3D identity{};	//単位変換情報
	identity.position = { 0.0f, 0.0f, 0.0f };
	identity.scale = { 1.0f, 1.0f, 1.0f };
	identity.rotation = { 0.0f, 0.0f, 0.0f };

	for(auto& root : m_roots) 
	{
		root->UpdateWorldTransform(identity);	//ルートUIオブジェクトのワールド変換情報を単位変換に設定
	}

	std::vector<WorldRenderInfo> renderInfos;	//描画情報構造体配列
	for(auto& root : m_roots) 
	{
		root->CollectRenderInfos(renderInfos);	//ルートUIオブジェクトの描画情報構造体配列を収集
	}

	SubmitRenderInfo(renderer, renderInfos);	//描画情報をシーンに提出
}

//終了
void UIManagerBase::Finalize()
{
	FinalizeOverride();

	for(auto& root : m_roots) 
	{
		root->Finalize();
	}

	m_roots.clear();

	EventManager::GetInstance()->Unsubscribe(EventType::START_FADE_IN, m_fadeStartEventID);
	EventManager::GetInstance()->Unsubscribe(EventType::START_FADE_OUT, m_fadeEndEventID);
}

//描画情報をシーンに提出
void UIManagerBase::SubmitRenderInfo(Renderer& renderer, std::vector<WorldRenderInfo>& info)
{
	for(auto& renderInfo : info) 
	{
		renderer.SubmitToScreenList(renderInfo);
	}
}

//オブジェクトの描画情報生成
void UIManagerBase::PrepareRenderInfo(
	TextureManager& textureManager,
	MeshManager& meshManager
)
{
	for (auto& root : m_roots)
	{
		root->PrepareRenderInfo(textureManager, meshManager);
	}
}

//フェードイン開始
void UIManagerBase::StartFadeIn(float duration)
{
	m_isFading = true;
	m_isFadeEnd = false;
	m_pFadeImage->SetColor({ 0.0f, 0.0f, 0.0f, 1.0f });
	m_fadeDuration = -duration;
}

//フェードアウト開始
void UIManagerBase::StartFadeOut(float duration)
{
	m_isFading = true;
	m_isFadeEnd = false;
	m_pFadeImage->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });
	m_fadeDuration = duration;
}

//フェード中判定
bool UIManagerBase::IsFading() const
{
	return m_isFading;
}

//フェード終了判定
bool UIManagerBase::IsFadeEnd() const
{
	return m_isFadeEnd;
}

//フェード更新関数
void UIManagerBase::UpdateFade()
{
	if(!m_isFading) return;
	XMFLOAT4 color = m_pFadeImage->GetColor();
	color.w += m_fadeDuration;

	if(color.w < 0.0f)
	{
		color.w = 0.0f;
		m_isFading = false;
		m_isFadeEnd = true;
	}
	else if(color.w > 1.0f)
	{
		color.w = 1.0f;
		m_isFading = false;
		m_isFadeEnd = true;
	}

	m_pFadeImage->SetColor(color);
}