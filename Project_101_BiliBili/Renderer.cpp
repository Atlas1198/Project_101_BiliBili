#include "Renderer.h"
#include "Engine.h"
#include "App.h"
#include "d3dx12.h"
#include "SharedStruct.h"
#include "AssimpLoader.h"

using namespace DirectX;
using namespace RenderData;

//初期化
void Renderer::Initialize(ID3D12Device* pDevice, CameraInfo* pInfo)
{
	m_pDevice = pDevice;	//デバイスの保存
	m_cameraInfo = pInfo;	//カメラ情報構造体の保存

	//定数バッファの生成
	for (int i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		//定数バッファの生成
		m_pConstantBuffer[i] = new ConstantBuffer(pDevice, sizeof(Transform));

		//定数バッファのポインタを取得
		auto ptr = m_pConstantBuffer[i]->GetPtr<Transform>();

		//ワールド行列は単位行列
		ptr->worldMatrix = XMMatrixIdentity();

		//ビュー行列の計算
		m_view = DirectX::XMMatrixLookAtLH(
			DirectX::XMVectorSet(pInfo->position.x, pInfo->position.y, pInfo->position.z, 0.0f),	//カメラの位置
			DirectX::XMVectorSet(pInfo->target.x, pInfo->target.y, pInfo->target.z, 0.0f),			//カメラの注視点
			DirectX::XMVectorSet(pInfo->up.x, pInfo->up.y, pInfo->up.z, 0.0f));						//カメラの上方向ベクトル

		//定数バッファにビュー行列をセット
		ptr->viewMatrix = m_view;

		//プロジェクション行列の計算
		m_proj = XMMatrixPerspectiveFovLH(
			pInfo->fov,			//垂直視野角
			pInfo->aspectRatio,	//アスペクト比
			pInfo->nearZ,		//ニアクリップ距離
			pInfo->farZ			//ファークリップ距離
		);

		//定数バッファにプロジェクション行列をセット
		ptr->projMatrix = m_proj;
	}

	//ルートシグネチャの生成
	m_pRootSignature = new RootSignature(m_pDevice);

	//パイプラインステートの生成
	m_pPipelineState = new PipelineState(m_pDevice);
	m_pPipelineState->SetInputLayout(Vertex::InputLayout);						//入力レイアウトの設定
	m_pPipelineState->SetRootSignature(m_pRootSignature->GetRootSignature());	//ルートシグネチャの設定
	m_pPipelineState->SetVertexShader(L"VertexShader.hlsl");					//頂点シェーダーの設定
	m_pPipelineState->SetPixelShader(L"PixelShader.hlsl");						//ピクセルシェーダーの設定
	m_pPipelineState->Create();													//パイプラインステートオブジェクトの作成
}

//更新
void Renderer::Update(UINT currentBackBufferIndex, CameraInfo& info)
{
	//カメラ行列の更新
	m_view = DirectX::XMMatrixLookAtLH(
		DirectX::XMVectorSet(info.position.x, info.position.y, info.position.z, 0.0f),	//カメラの位置
		DirectX::XMVectorSet(info.target.x, info.target.y, info.target.z, 0.0f),			//カメラの注視点
		DirectX::XMVectorSet(info.up.x, info.up.y, info.up.z, 0.0f));					//カメラの上方向ベクトル

	//プロジェクション行列の更新
	m_proj = XMMatrixPerspectiveFovLH(
		info.fov,			//垂直視野角
		info.aspectRatio,	//アスペクト比
		info.nearZ,		//ニアクリップ距離
		info.farZ			//ファークリップ距離
	);
}

//描画
void Renderer::Draw(
	UINT index,									//描画インデックス（未使用）
	ID3D12GraphicsCommandList* p_commandList,	//コマンドリスト
	TextureManager& textureManager				//テクスチャ管理クラス
)
{
	//デスクリプタヒープの設定
	ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap() };	//SRVヒープの取得
	p_commandList->SetDescriptorHeaps(_countof(heaps), heaps);			//デスクリプタヒープの設定

	//ルートシグネチャとパイプラインステートの設定
	p_commandList->SetGraphicsRootSignature(m_pRootSignature->GetRootSignature());
	p_commandList->SetPipelineState(m_pPipelineState->GetPipelineState());

	// ドロー要求を順に処理
	for (size_t i = 0; i < m_drawList.size(); ++i)
	{
		// フレームごとのCBVプールを必要数まで確保
		if (i >= m_objectCB[m_currBackIndex].size()) 
		{
			//新しい定数バッファを作成
			auto* newCb = new ConstantBuffer(m_pDevice, sizeof(Transform));

			if (!newCb->GetIsValid()) 
			{//作成失敗時
				OutputDebugStringA("ConstantBuffer creation failed\n");
				delete newCb;
			}

			//プールに追加
			m_objectCB[m_currBackIndex].push_back(newCb);
		}

		//オブジェクト用定数バッファの取得
		ConstantBuffer* cb = m_objectCB[m_currBackIndex][i];
		auto* ptr = cb->GetPtr<Transform>();

		//定数バッファに transform を書く（各オブジェクト専用のメモリ）
		ptr->worldMatrix = m_drawList[i].world;	//ワールド行列
		ptr->viewMatrix = m_view;				//ビュー行列
		ptr->projMatrix = m_proj;				//プロジェクション行列
		ptr->objectColor = m_drawList[i].color;	//オブジェクトの色

		//メッシュGPUデータの取得
		auto meshGPU = m_drawList[i].pMeshGPU;

		//セットアップ
		auto vbv = meshGPU->GetVertexBuffer()->GetView();						//頂点バッファビューの取得
		auto ibv = meshGPU->GetIndexBuffer()->GetView();						//インデックスバッファビューの取得
		p_commandList->SetGraphicsRootConstantBufferView(0, cb->GetAddress());	//ルートパラメータ0に定数バッファをセット
		p_commandList->IASetPrimitiveTopology(meshGPU->GetTopology());			//プリミティブトポロジの設定
		p_commandList->IASetVertexBuffers(0, 1, &vbv);							//頂点バッファの設定
		p_commandList->IASetIndexBuffer(&ibv);									//インデックスバッファの設定
		
		//SRVの設定
		if(m_drawList[i].srvIndex != UINT32_MAX)
		{//SRVインデックスが有効な場合
			auto gpuHandle = textureManager.GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();						//SRVヒープのGPUハンドルを取得
			gpuHandle.ptr += static_cast<UINT64>(m_drawList[i].srvIndex) * textureManager.GetSrvIncrementSize();	//SRVインデックスに対応するGPUハンドルを計算
			p_commandList->SetGraphicsRootDescriptorTable(1, gpuHandle);											//t0にSRVをセット
		}
		
		//描画コマンドの発行
		p_commandList->DrawIndexedInstanced(	//描画コマンド
			meshGPU->GetIndexCount(),	//インデックス数
			1,							//インスタンス数
			m_drawList[i].startIndex,	//スタートインデックス位置
			m_drawList[i].baseVertex,	//ベース頂点位置
			0							//スタートインスタンス位置
		);
	}
}

//フレーム開始
void Renderer::BeginFrame(UINT backIndex)
{
	m_currBackIndex = backIndex;	//現在のバックバッファインデックスを保存
	m_drawList.clear();				//描画リストをクリア
}

//描画リストに描画要求を追加
void Renderer::Submit(const RenderInfo& item)
{
	//wchar_t buf[128];
	//swprintf_s(buf, L"Submit: srvIndex = %u\n", item.srvIndex);
	//OutputDebugStringW(buf);
	m_drawList.push_back(item);	//描画リストに描画要求を追加
}
