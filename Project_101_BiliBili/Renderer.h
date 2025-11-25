#pragma once
#include "Engine.h"
#include "ComPtr.h"
#include "VertexBuffer.h"
#include "ConstantBuffer.h"
#include "RootSignature.h"
#include "SharedStruct.h"
#include "PipelineState.h"
#include "IndexBuffer.h"
#include "TextureManager.h"

//レンダラークラス
class Renderer
{
public:	//公開関数
	Renderer() {};	//コンストラクタ
	~Renderer();	//デストラクタ

	//メイン処理関数	
	void Initialize(											//初期化
		ID3D12Device* pDevice, 
		CameraInfo* pInfo);		
	void Update(UINT currentBackBufferIndex, CameraInfo& info);	//更新
	void Draw(													//描画
		UINT index, 
		ID3D12GraphicsCommandList* commandList,
		TextureManager& textureManager
	);

	//描画リスト管理関数
	void BeginFrame(UINT backIndex);

	//描画リストに描画情報を追加
	void SubmitToWorldList(const struct RenderData::RenderInfo& item);	//ワールド座標用
	void SubmitToScreenList(const struct RenderData::RenderInfo& item);	//スクリーン座標用

private:	//非公開メンバ変数
	RootSignature* m_pRootSignature = nullptr;							//ルートシグネチャ
	PipelineState* m_pPipelineStateWorld[BLEND_MAX]{};					//ワールド座標用パイプラインステートオブジェクト
	PipelineState* m_pPipelineStateScreen[BLEND_MAX]{};					//スクリーン座標用パイプラインステートオブジェクト

	ID3D12Device* m_pDevice = nullptr;	//デバイス
	CameraInfo* m_cameraInfo = nullptr;	//カメラ情報構造体

	std::vector<RenderData::RenderInfo> m_drawListWorld[BLEND_MAX]{};	//描画リスト(ワールド座標)
	std::vector<RenderData::RenderInfo> m_drawListScreen[BLEND_MAX]{};	//描画リスト(スクリーン座標)

	//フレームごとのオブジェクト用CBVプール（1オブジェクト＝1定数バッファ）
	std::vector<ConstantBuffer*> m_objectCBWorld[Engine::FRAME_BUFFER_COUNT];	//ワールド座標用
	std::vector<ConstantBuffer*> m_objectCBScreen[Engine::FRAME_BUFFER_COUNT];	//スクリーン座標用
	UINT m_currBackIndex = 0;

	//カメラ行列
	DirectX::XMMATRIX m_worldView{};	//ワールド座標用ビュー行列
	DirectX::XMMATRIX m_worldProj{};	//ワールド座標用プロジェクション行列
	DirectX::XMMATRIX m_screenProj{};	//スクリーン座標用プロジェクション行列
	DirectX::XMMATRIX m_screenView{};	//スクリーン座標用ビュー行列


private:	//非公開関数
	//描画リストの描画関数
	void DrawRenderListWorld(	//ワールド座標用描画リストの描画
		ID3D12GraphicsCommandList* p_commandList,	//コマンドリスト
		TextureManager& textureManager,				//テクスチャ管理クラス
		size_t objIndex								//オブジェクト用定数バッファのインデックス
	);
	void DrawRenderListScreen(	//スクリーン座標用描画リストの描画
		ID3D12GraphicsCommandList* p_commandList,	//コマンドリスト
		TextureManager& textureManager,				//テクスチャ管理クラス
		size_t objIndex								//オブジェクト用定数バッファのインデックス
	);

	//描画リストソート関数
	void SortDrawList();			//描画リストのソート
	void SortDrawListOpaque();		//不透明オブジェクトの描画リストソート
	void SortDrawListTransparent();	//透明オブジェクトの描画リストソート
};