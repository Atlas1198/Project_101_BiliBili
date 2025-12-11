#include "UIManagerBase.h"
#include "Renderer.h"
#include "TextureManager.h"
#include "MeshManager.h"
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

	PrepareRenderInfo(textureManager, meshManager);
}

//更新
void UIManagerBase::Update()
{
	UpdateOverride();
	for(auto& root : m_roots) 
	{
		root->Update();
	}
}

//描画要求をシーンに提出
void UIManagerBase::SubmitDraws(Renderer& renderer)
{
	Transform3D identity{};	//単位変換情報
	identity.position = { 0.0f, 0.0f, 0.0f };
	identity.scale = { 1.0f, 1.0f, 1.0f };
	identity.rotation = { 0.0f, 0.0f, 0.0f };

	for(auto& root : m_roots) 
	{
		root->UpdateWorldTransform(identity);	//ルートUIオブジェクトのワールド変換情報を単位変換に設定
	}

	std::vector<::RenderInfo> renderInfos;	//描画情報構造体配列
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
}

//描画情報をシーンに提出
void UIManagerBase::SubmitRenderInfo(Renderer& renderer, std::vector<::RenderInfo>& info)
{
	for(auto& renderInfo : info) 
	{
		renderer.SubmitToScreenList(renderInfo);
	}
}
