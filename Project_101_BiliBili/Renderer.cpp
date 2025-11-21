#include "Renderer.h"
#include <algorithm>
#include <vector>
#include "Engine.h"
#include "App.h"
#include "d3dx12.h"
#include "SharedStruct.h"
#include "AssimpLoader.h"

using namespace DirectX;
using namespace RenderData;

//デストラクタ
Renderer::~Renderer()
{
	//ルートシグネチャの解放
	if (m_pRootSignature)
	{
		delete m_pRootSignature;
		m_pRootSignature = nullptr;
	}
	//パイプラインステートの解放
	for (auto& pPipelineState : m_pPipelineState)
	{
		if (pPipelineState)
		{
			delete pPipelineState;
			pPipelineState = nullptr;
		}
	}
	//定数バッファの解放
	for (int i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		if (m_pConstantBuffer[i])
		{
			delete m_pConstantBuffer[i];
			m_pConstantBuffer[i] = nullptr;
		}
	}
}

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

		//プロジェクション行列の更新
		m_proj = XMMatrixPerspectiveFovLH(
			pInfo->fov,			//視野角
			pInfo->aspectRatio,	//アスペクト比
			pInfo->nearZ,			//ニアクリップ距離
			pInfo->farZ			//ファークリップ距離
		);

		//定数バッファにプロジェクション行列をセット
		ptr->projMatrix = m_proj;
	}

	//ルートシグネチャの生成
	m_pRootSignature = new RootSignature(m_pDevice);

	//パイプラインステートの生成
	for(auto& pPipelineState : m_pPipelineState)
	{
		pPipelineState = new PipelineState(m_pDevice);
		pPipelineState->SetInputLayout(Vertex::InputLayout);						//入力レイアウトの設定
		pPipelineState->SetRootSignature(m_pRootSignature->GetRootSignature());	//ルートシグネチャの設定
		pPipelineState->SetVertexShader(L"VertexShader.hlsl");					//頂点シェーダーの設定
	}

	//不透明設定
	m_pPipelineState[BLEND_OPAQUE]->SetPixelShader(L"PixelShader.hlsl", "BasicPS");	//ピクセルシェーダーの設定
	m_pPipelineState[BLEND_OPAQUE]->EnableAlphaBlend(false);						//不透明設定
	m_pPipelineState[BLEND_OPAQUE]->EnableDepthWrite(true);							//深度書き込み有効
	m_pPipelineState[BLEND_OPAQUE]->Create();										//生成
	//マスク設定
	m_pPipelineState[BLEND_MASKED]->SetPixelShader(L"PixelShader.hlsl", "BasicPSMasked");	//ピクセルシェーダーの設定
	m_pPipelineState[BLEND_MASKED]->EnableAlphaBlend(false);								//不透明設定
	m_pPipelineState[BLEND_MASKED]->EnableDepthWrite(true);									//深度書き込み有効
	m_pPipelineState[BLEND_MASKED]->Create();												//生成
	//透明設定
	m_pPipelineState[BLEND_TRANSPARENT]->SetPixelShader(L"PixelShader.hlsl", "BasicPS");	//ピクセルシェーダーの設定
	m_pPipelineState[BLEND_TRANSPARENT]->EnableAlphaBlend(true);					//透明設定
	m_pPipelineState[BLEND_TRANSPARENT]->EnableDepthWrite(false);					//深度書き込み無効
	m_pPipelineState[BLEND_TRANSPARENT]->Create();									//生成
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
		info.fov,			//視野角
		info.aspectRatio,	//アスペクト比
		info.nearZ,			//ニアクリップ距離
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
	SortDrawList();	//描画リストのソート

	//デスクリプタヒープの設定
	ID3D12DescriptorHeap* heaps[] = { textureManager.GetSrvHeap() };	//SRVヒープの取得
	p_commandList->SetDescriptorHeaps(_countof(heaps), heaps);			//デスクリプタヒープの設定

	//ルートシグネチャの設定
	p_commandList->SetGraphicsRootSignature(m_pRootSignature->GetRootSignature());

	size_t objIndex = 0;	//オブジェクト用定数バッファのインデックス

	// ドロー要求を順に処理
	for (size_t i = 0; i < BLEND_MAX; ++i)
	{
		//パイプラインステートの設定
		p_commandList->SetPipelineState(m_pPipelineState[i]->GetPipelineState());

		for(size_t j = 0; j < m_drawList[i].size(); j++)
		{
			// フレームごとのCBVプールを必要数まで確保
			if (objIndex >= m_objectCB[m_currBackIndex].size())
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
			ConstantBuffer* cb = m_objectCB[m_currBackIndex][objIndex];
			auto* ptr = cb->GetPtr<Transform>();

			//定数バッファに transform を書く（各オブジェクト専用のメモリ）
			ptr->worldMatrix = m_drawList[i][j].world;	//ワールド行列
			ptr->viewMatrix = m_view;					//ビュー行列
			ptr->projMatrix = m_proj;					//プロジェクション行列
			ptr->objectColor = m_drawList[i][j].color;	//オブジェクトの色

			//メッシュGPUデータの取得
			auto meshGPU = m_drawList[i][j].pMeshGPU;

			//セットアップ
			auto vbv = meshGPU->GetVertexBuffer()->GetView();						//頂点バッファビューの取得
			auto ibv = meshGPU->GetIndexBuffer()->GetView();						//インデックスバッファビューの取得
			p_commandList->SetGraphicsRootConstantBufferView(0, cb->GetAddress());	//ルートパラメータ0に定数バッファをセット
			p_commandList->IASetPrimitiveTopology(meshGPU->GetTopology());			//プリミティブトポロジの設定
			p_commandList->IASetVertexBuffers(0, 1, &vbv);							//頂点バッファの設定
			p_commandList->IASetIndexBuffer(&ibv);									//インデックスバッファの設定

			//SRVの設定
			if (m_drawList[i][j].srvIndex != UINT32_MAX)
			{//SRVインデックスが有効な場合
				auto gpuHandle = textureManager.GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();						//SRVヒープのGPUハンドルを取得
				gpuHandle.ptr += static_cast<UINT64>(m_drawList[i][j].srvIndex) * textureManager.GetSrvIncrementSize();	//SRVインデックスに対応するGPUハンドルを計算
				p_commandList->SetGraphicsRootDescriptorTable(1, gpuHandle);											//t0にSRVをセット
			}

			//描画コマンドの発行
			p_commandList->DrawIndexedInstanced(	//描画コマンド
				meshGPU->GetIndexCount(),		//インデックス数
				1,								//インスタンス数
				m_drawList[i][j].startIndex,	//スタートインデックス位置
				m_drawList[i][j].baseVertex,	//ベース頂点位置
				0								//スタートインスタンス位置
			);

			objIndex++;	//オブジェクト用定数バッファのインデックスを進める
		}
	}
}

//フレーム開始
void Renderer::BeginFrame(UINT backIndex)
{
	m_currBackIndex = backIndex;	//現在のバックバッファインデックスを保存
	//描画リストのクリア
	for(auto& drawList : m_drawList)
	{
		drawList.clear();
	}
}

//描画リストに描画要求を追加
void Renderer::Submit(const RenderInfo& item)
{
	m_drawList[item.blendMode].push_back(item);	//描画リストに描画要求を追加
}

//描画リストのソート
void Renderer::SortDrawList()
{
	SortDrawListOpaque();			//不透明オブジェクトの描画リストソート
	SortDrawListTransparent();		//透明オブジェクトの描画リストソート
}

//不透明オブジェクトの描画リストソート
void Renderer::SortDrawListOpaque()
{
	auto cameraPos = m_cameraInfo->position;	//カメラ位置

	//距離の二乗を計算するラムダ式
	auto dist2 = [](const XMFLOAT3& a, const XMFLOAT3& b)
		{
			float dx = a.x - b.x;
			float dy = a.y - b.y;
			float dz = a.z - b.z;
			return dx * dx + dy * dy + dz * dz;
		};
	//カメラから近い順にソート
	//OPAQUE
	std::sort(
		m_drawList[BLEND_OPAQUE].begin(),	//ソート開始位置
		m_drawList[BLEND_OPAQUE].end(),		//ソート終了位置
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return dist2(a.positionW, cameraPos) > dist2(b.positionW, cameraPos);
		}
	);
	//MASKED
	std::sort(
		m_drawList[BLEND_MASKED].begin(),	//ソート開始位置
		m_drawList[BLEND_MASKED].end(),		//ソート終了位置
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return dist2(a.positionW, cameraPos) > dist2(b.positionW, cameraPos);
		}
	);
}

//透明オブジェクトの描画リストソート
void Renderer::SortDrawListTransparent()
{
	//カメラ位置と正面ベクトルの計算
	XMFLOAT3 cameraPos = m_cameraInfo->position;	//カメラ位置
	XMFLOAT3 cameraForward = {						//カメラ正面ベクトル
		m_cameraInfo->target.x - m_cameraInfo->position.x,
		m_cameraInfo->target.y - m_cameraInfo->position.y,
		m_cameraInfo->target.z - m_cameraInfo->position.z
	};

	//正規化
	XMVECTOR vF = XMVector3Normalize(XMLoadFloat3(&cameraForward));
	XMStoreFloat3(&cameraForward, vF);

	auto depthFar = [&](const RenderInfo& r)
	{
		//カメラからの奥行きを計算
		float vx = r.positionW.x - cameraPos.x;
		float vy = r.positionW.y - cameraPos.y;
		float vz = r.positionW.z - cameraPos.z;
		float centerDepth =
			vx * cameraForward.x + vy * cameraForward.y + vz * cameraForward.z;

		//ソート用の半径を足す
		float radius = r.pMeshGPU->GetSortRadius();
		return centerDepth + radius;
	};

	//カメラから遠い順にソート
	std::sort(
		m_drawList[BLEND_TRANSPARENT].begin(),
		m_drawList[BLEND_TRANSPARENT].end(),
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return depthFar(a) > depthFar(b);
		}
	);
}
