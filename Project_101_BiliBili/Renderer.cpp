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
	for (auto& pPipelineState : m_pPipelineStateWorld)
	{
		if (pPipelineState)
		{
			delete pPipelineState;
			pPipelineState = nullptr;
		}
	}
	//オブジェクト用定数バッファの解放
	for (int i = 0; i < Engine::FRAME_BUFFER_COUNT; i++)
	{
		for (auto& pCB : m_objectCBWorld[i])
		{
			if (pCB)
			{
				delete pCB;
				pCB = nullptr;
			}
		}
		m_objectCBWorld[i].clear();
	}
}

//初期化
void Renderer::Initialize(ID3D12Device* pDevice, CameraInfo* pInfo)
{
	m_pDevice = pDevice;	//デバイスの保存
	m_cameraInfo = pInfo;	//カメラ情報構造体の保存

	//ルートシグネチャの生成
	m_pRootSignature = new RootSignature(m_pDevice);

	//パイプラインステートの生成
	for(auto& pPipelineState : m_pPipelineStateWorld)
	{
		pPipelineState = new PipelineState(m_pDevice);
		pPipelineState->SetInputLayout(Vertex::InputLayout);						//入力レイアウトの設定
		pPipelineState->SetRootSignature(m_pRootSignature->GetRootSignature());	//ルートシグネチャの設定
		pPipelineState->SetVertexShader(L"VertexShader.hlsl");					//頂点シェーダーの設定
	}
	for(auto& pPipelineState : m_pPipelineStateScreen)
	{
		pPipelineState = new PipelineState(m_pDevice);
		pPipelineState->SetInputLayout(Vertex::InputLayout);						//入力レイアウトの設定
		pPipelineState->SetRootSignature(m_pRootSignature->GetRootSignature());	//ルートシグネチャの設定
		pPipelineState->SetVertexShader(L"VertexShader.hlsl");					//頂点シェーダーの設定
	}

	//ワールド座標用パイプラインステートの設定
	//不透明設定
	m_pPipelineStateWorld[BLEND_OPAQUE]->SetPixelShader(L"PixelShader.hlsl", "BasicPS");	//ピクセルシェーダーの設定
	m_pPipelineStateWorld[BLEND_OPAQUE]->EnableAlphaBlend(false);							//不透明設定
	m_pPipelineStateWorld[BLEND_OPAQUE]->EnableDepthWrite(true);							//深度書き込み有効
	m_pPipelineStateWorld[BLEND_OPAQUE]->Create();											//生成
	//マスク設定
	m_pPipelineStateWorld[BLEND_MASKED]->SetPixelShader(L"PixelShader.hlsl", "BasicPSMasked");	//ピクセルシェーダーの設定
	m_pPipelineStateWorld[BLEND_MASKED]->EnableAlphaBlend(false);								//不透明設定
	m_pPipelineStateWorld[BLEND_MASKED]->EnableDepthWrite(true);								//深度書き込み有効
	m_pPipelineStateWorld[BLEND_MASKED]->Create();												//生成
	//透明設定
	m_pPipelineStateWorld[BLEND_TRANSPARENT]->SetPixelShader(L"PixelShader.hlsl", "BasicPS");	//ピクセルシェーダーの設定
	m_pPipelineStateWorld[BLEND_TRANSPARENT]->EnableAlphaBlend(true);							//透明設定
	m_pPipelineStateWorld[BLEND_TRANSPARENT]->EnableDepthWrite(false);							//深度書き込み無効
	m_pPipelineStateWorld[BLEND_TRANSPARENT]->Create();											//生成

	//スクリーン座標用パイプラインステートの設定
	//不透明設定
	m_pPipelineStateScreen[BLEND_OPAQUE]->SetPixelShader(L"PixelShader.hlsl", "BasicPS");	//ピクセルシェーダーの設定
	m_pPipelineStateScreen[BLEND_OPAQUE]->EnableAlphaBlend(false);							//不透明設定
	m_pPipelineStateScreen[BLEND_OPAQUE]->EnableDepthWrite(false);							//深度書き込み有効
	m_pPipelineStateScreen[BLEND_OPAQUE]->EnableDepthTest(false);							//深度テスト無効
	m_pPipelineStateScreen[BLEND_OPAQUE]->SetCullMode(D3D12_CULL_MODE_NONE);				//カリング無効化
	m_pPipelineStateScreen[BLEND_OPAQUE]->Create();											//生成
	//マスク設定
	m_pPipelineStateScreen[BLEND_MASKED]->SetPixelShader(L"PixelShader.hlsl", "BasicPSMasked");	//ピクセルシェーダーの設定
	m_pPipelineStateScreen[BLEND_MASKED]->EnableAlphaBlend(false);								//不透明設定
	m_pPipelineStateScreen[BLEND_MASKED]->EnableDepthWrite(false);								//深度書き込み有効
	m_pPipelineStateScreen[BLEND_MASKED]->EnableDepthTest(false);								//深度テスト無効
	m_pPipelineStateScreen[BLEND_MASKED]->SetCullMode(D3D12_CULL_MODE_NONE);					//カリング無効化
	m_pPipelineStateScreen[BLEND_MASKED]->Create();												//生成
	//透明設定
	m_pPipelineStateScreen[BLEND_TRANSPARENT]->SetPixelShader(L"PixelShader.hlsl", "BasicPS");		//ピクセルシェーダーの設定
	m_pPipelineStateScreen[BLEND_TRANSPARENT]->EnableAlphaBlend(true);								//透明設定
	m_pPipelineStateScreen[BLEND_TRANSPARENT]->EnableDepthWrite(false);								//深度書き込み無効
	m_pPipelineStateScreen[BLEND_TRANSPARENT]->EnableDepthTest(false);								//深度テスト無効
	m_pPipelineStateScreen[BLEND_TRANSPARENT]->SetCullMode(D3D12_CULL_MODE_NONE);					//カリング無効化
	m_pPipelineStateScreen[BLEND_TRANSPARENT]->Create();											//生成
}

//更新
void Renderer::Update(UINT currentBackBufferIndex, CameraInfo& info)
{
	//ワールドカメラ行列の更新
	m_worldView = DirectX::XMMatrixLookAtLH(
		XMVectorSet(info.position.x, info.position.y, info.position.z, 0.0f),	//カメラの位置
		XMVectorSet(info.target.x, info.target.y, info.target.z, 0.0f),			//カメラの注視点
		XMVectorSet(info.up.x, info.up.y, info.up.z, 0.0f));					//カメラの上方向ベクトル

	//ワールドプロジェクション行列の更新
	/*
	m_worldProj = XMMatrixPerspectiveFovLH(
		info.fov,			//視野角
		info.aspectRatio,	//アスペクト比
		info.nearZ,			//ニアクリップ距離
		info.farZ			//ファークリップ距離
	);
	*/

	float orthoheight = 30;
	float orthowidth = orthoheight * info.aspectRatio;

	m_worldProj = XMMatrixOrthographicLH(
		orthowidth,	//画面幅
		orthoheight,	//画面高さ5
		0.1f,									//ニアクリップ距離
		100.0f);

	//スクリーンカメラ行列の更新
	m_screenView = XMMatrixIdentity();					//カメラの上方

	

	//スクリーンプロジェクション行列の更新
	m_screenProj = XMMatrixOrthographicLH(
		(int)App::WINDOW_WIDTH,	//画面幅
		(int)App::WINDOW_HEIGHT,	//画面高さ
		0.0f,									//ニアクリップ距離
		1.0f);									//ファークリップ距離
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

	DrawRenderListWorld(p_commandList, textureManager, objIndex);	//ワールド座標用描画リストの描画
	DrawRenderListScreen(p_commandList, textureManager, objIndex);	//スクリーン座標用描画リストの描画
}

//フレーム開始
void Renderer::BeginFrame(UINT backIndex)
{
	m_currBackIndex = backIndex;	//現在のバックバッファインデックスを保存
	//描画リストのクリア
	for(auto& drawList : m_drawListWorld)
	{
		drawList.clear();
	}
	for(auto& drawList : m_drawListScreen)
	{
		drawList.clear();
	}
}

//ワールド座標用描画リストに描画要求を追加
void Renderer::SubmitToWorldList(const RenderInfo& item)
{
	m_drawListWorld[item.blendMode].push_back(item);	//描画リストに描画要求を追加
}

//スクリーン座標用描画リストに描画要求を追加
void Renderer::SubmitToScreenList(const RenderData::RenderInfo& item)
{
	m_drawListScreen[item.blendMode].push_back(item);	//描画リストに描画要求を追加
}

//スクリーン座標用描画リストの描画
void Renderer::DrawRenderListWorld(
	ID3D12GraphicsCommandList* p_commandList, 
	TextureManager& textureManager,
	size_t objIndex
)
{
	// ドロー要求を順に処理
	for (size_t i = 0; i < BLEND_MAX; ++i)
	{
		//パイプラインステートの設定
		p_commandList->SetPipelineState(m_pPipelineStateWorld[i]->GetPipelineState());

		for (size_t j = 0; j < m_drawListWorld[i].size(); j++)
		{
			// フレームごとのCBVプールを必要数まで確保
			if (objIndex >= m_objectCBWorld[m_currBackIndex].size())
			{
				//新しい定数バッファを作成
				auto* newCb = new ConstantBuffer(m_pDevice, sizeof(Transform));

				if (!newCb->GetIsValid())
				{//作成失敗時
					OutputDebugStringA("ConstantBuffer creation failed\n");
					delete newCb;
				}

				//プールに追加
				m_objectCBWorld[m_currBackIndex].push_back(newCb);
			}

			//オブジェクト用定数バッファの取得
			ConstantBuffer* cb = m_objectCBWorld[m_currBackIndex][objIndex];
			auto* ptr = cb->GetPtr<Transform>();

			//定数バッファに transform を書く（各オブジェクト専用のメモリ）
			ptr->worldMatrix = m_drawListWorld[i][j].world;	//ワールド行列
			ptr->viewMatrix = m_worldView;					//ビュー行列
			ptr->projMatrix = m_worldProj;					//プロジェクション行列
			ptr->objectColor = m_drawListWorld[i][j].color;	//オブジェクトの色
			ptr->uvRect = m_drawListWorld[i][j].uvRect;		//UV矩形

			//メッシュGPUデータの取得
			auto meshGPU = m_drawListWorld[i][j].pMeshGPU;

			//セットアップ
			auto vbv = meshGPU->GetVertexBuffer()->GetView();						//頂点バッファビューの取得
			auto ibv = meshGPU->GetIndexBuffer()->GetView();						//インデックスバッファビューの取得
			p_commandList->SetGraphicsRootConstantBufferView(0, cb->GetAddress());	//ルートパラメータ0に定数バッファをセット
			p_commandList->IASetPrimitiveTopology(meshGPU->GetTopology());			//プリミティブトポロジの設定
			p_commandList->IASetVertexBuffers(0, 1, &vbv);							//頂点バッファの設定
			p_commandList->IASetIndexBuffer(&ibv);									//インデックスバッファの設定

			//SRVの設定
			if (m_drawListWorld[i][j].srvIndex != UINT32_MAX)
			{//SRVインデックスが有効な場合
				auto gpuHandle = textureManager.GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();						//SRVヒープのGPUハンドルを取得
				gpuHandle.ptr += static_cast<UINT64>(m_drawListWorld[i][j].srvIndex) * textureManager.GetSrvIncrementSize();	//SRVインデックスに対応するGPUハンドルを計算
				p_commandList->SetGraphicsRootDescriptorTable(1, gpuHandle);											//t0にSRVをセット
			}

			//描画コマンドの発行
			p_commandList->DrawIndexedInstanced(	//描画コマンド
				meshGPU->GetIndexCount(),		//インデックス数
				1,								//インスタンス数
				m_drawListWorld[i][j].startIndex,	//スタートインデックス位置
				m_drawListWorld[i][j].baseVertex,	//ベース頂点位置
				0								//スタートインスタンス位置
			);

			objIndex++;	//オブジェクト用定数バッファのインデックスを進める
		}
	}
}

//スクリーン座標用描画リストの描画
void Renderer::DrawRenderListScreen(
	ID3D12GraphicsCommandList* p_commandList, 
	TextureManager& textureManager,
	size_t objIndex
)
{
	// ドロー要求を順に処理
	for (size_t i = 0; i < BLEND_MAX; ++i)
	{
		//パイプラインステートの設定
		p_commandList->SetPipelineState(m_pPipelineStateScreen[i]->GetPipelineState());

		for (size_t j = 0; j < m_drawListScreen[i].size(); j++)
		{
			// フレームごとのCBVプールを必要数まで確保
			if (objIndex >= m_objectCBScreen[m_currBackIndex].size())
			{
				//新しい定数バッファを作成
				auto* newCb = new ConstantBuffer(m_pDevice, sizeof(Transform));

				if (!newCb->GetIsValid())
				{//作成失敗時
					OutputDebugStringA("ConstantBuffer creation failed\n");
					delete newCb;
				}

				//プールに追加
				m_objectCBScreen[m_currBackIndex].push_back(newCb);
			}

			//オブジェクト用定数バッファの取得
			ConstantBuffer* cb = m_objectCBScreen[m_currBackIndex][objIndex];
			auto* ptr = cb->GetPtr<Transform>();

			//定数バッファに transform を書く（各オブジェクト専用のメモリ）
			ptr->worldMatrix = m_drawListScreen[i][j].world;	//ワールド行列
			ptr->viewMatrix = m_screenView;						//ビュー行列
			ptr->projMatrix = m_screenProj;						//プロジェクション行列
			ptr->objectColor = m_drawListScreen[i][j].color;	//オブジェクトの色
			ptr->uvRect = m_drawListScreen[i][j].uvRect;		//UV矩形

			//メッシュGPUデータの取得
			auto meshGPU = m_drawListScreen[i][j].pMeshGPU;

			//セットアップ
			auto vbv = meshGPU->GetVertexBuffer()->GetView();						//頂点バッファビューの取得
			auto ibv = meshGPU->GetIndexBuffer()->GetView();						//インデックスバッファビューの取得
			p_commandList->SetGraphicsRootConstantBufferView(0, cb->GetAddress());	//ルートパラメータ0に定数バッファをセット
			p_commandList->IASetPrimitiveTopology(meshGPU->GetTopology());			//プリミティブトポロジの設定
			p_commandList->IASetVertexBuffers(0, 1, &vbv);							//頂点バッファの設定
			p_commandList->IASetIndexBuffer(&ibv);									//インデックスバッファの設定

			//SRVの設定
			if (m_drawListScreen[i][j].srvIndex != UINT32_MAX)
			{//SRVインデックスが有効な場合
				auto gpuHandle = textureManager.GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();						//SRVヒープのGPUハンドルを取得
				gpuHandle.ptr += static_cast<UINT64>(m_drawListScreen[i][j].srvIndex) * textureManager.GetSrvIncrementSize();	//SRVインデックスに対応するGPUハンドルを計算
				p_commandList->SetGraphicsRootDescriptorTable(1, gpuHandle);											//t0にSRVをセット
			}

			//描画コマンドの発行
			p_commandList->DrawIndexedInstanced(	//描画コマンド
				meshGPU->GetIndexCount(),		//インデックス数
				1,								//インスタンス数
				m_drawListScreen[i][j].startIndex,	//スタートインデックス位置
				m_drawListScreen[i][j].baseVertex,	//ベース頂点位置
				0								//スタートインスタンス位置
			);

			objIndex++;	//オブジェクト用定数バッファのインデックスを進める
		}
	}
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
	//ワールド座標用描画リスト
	//OPAQUE
	std::sort(
		m_drawListWorld[BLEND_OPAQUE].begin(),	//ソート開始位置
		m_drawListWorld[BLEND_OPAQUE].end(),	//ソート終了位置
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return dist2(a.positionW, cameraPos) < dist2(b.positionW, cameraPos);
		}
	);
	//MASKED
	std::sort(
		m_drawListWorld[BLEND_MASKED].begin(),	//ソート開始位置
		m_drawListWorld[BLEND_MASKED].end(),	//ソート終了位置
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return dist2(a.positionW, cameraPos) < dist2(b.positionW, cameraPos);
		}
	);

	//スクリーン座標用描画リスト
	//OPAQUE
	std::sort(
		m_drawListScreen[BLEND_OPAQUE].begin(),	//ソート開始位置
		m_drawListScreen[BLEND_OPAQUE].end(),	//ソート終了位置
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return dist2(a.positionW, cameraPos) < dist2(b.positionW, cameraPos);
		}
	);
	//MASKED
	std::sort(
		m_drawListScreen[BLEND_MASKED].begin(),	//ソート開始位置
		m_drawListScreen[BLEND_MASKED].end(),	//ソート終了位置
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return dist2(a.positionW, cameraPos) < dist2(b.positionW, cameraPos);
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
	//ワールド座標用描画リスト
	std::sort(
		m_drawListWorld[BLEND_TRANSPARENT].begin(),
		m_drawListWorld[BLEND_TRANSPARENT].end(),
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return depthFar(a) > depthFar(b);
		}
	);
	
	//スクリーン座標用描画リスト
	std::sort(
		m_drawListScreen[BLEND_TRANSPARENT].begin(),
		m_drawListScreen[BLEND_TRANSPARENT].end(),
		[&](const RenderInfo& a, const RenderInfo& b)
		{
			return depthFar(a) > depthFar(b);
		}
	);
}
