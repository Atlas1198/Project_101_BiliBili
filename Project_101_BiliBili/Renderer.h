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

//シーンクラス
class Renderer
{
private:
	VertexBuffer* m_pVertexBuffer = nullptr;							//頂点バッファ
	ConstantBuffer* m_pConstantBuffer[Engine::FRAME_BUFFER_COUNT]{};	//定数バッファ
	RootSignature* m_pRootSignature = nullptr;							//ルートシグネチャ
	PipelineState* m_pPipelineState = nullptr;							//パイプラインステート
	IndexBuffer* m_pIndexBuffer = nullptr;								//インデックスバッファ

	ID3D12Device* m_pDevice = nullptr;	//デバイス
	CameraInfo* m_cameraInfo = nullptr;	//カメラ情報構造体

	std::vector<RenderData::RenderInfo> m_drawList;	//描画リスト

	// フレームごとのオブジェクト用CBVプール（1オブジェクト＝1定数バッファ）
	std::vector<ConstantBuffer*> m_objectCB[Engine::FRAME_BUFFER_COUNT];
	UINT m_currBackIndex = 0;

	//カメラ行列
	DirectX::XMMATRIX m_view{};	//ビュー行列
	DirectX::XMMATRIX m_proj{};	//プロジェクション行列

public:	//公開関数
	Renderer() {};		//コンストラクタ
	~Renderer() {};	//デストラクタ

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
	void Submit(const struct RenderData::RenderInfo& item);

	//
};