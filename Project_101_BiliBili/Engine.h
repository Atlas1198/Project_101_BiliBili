#pragma once
#include <tchar.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <DirectXMath.h>
#include <d3dcompiler.h>
#include "DirectXTex.h"
#include <vector>
#include <string>
#include "ComPtr.h"

#pragma comment(lib,"d3d12.lib")
#pragma comment(lib,"dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")

//DirectX12エンジン
class Engine
{
public:
	static constexpr int FRAME_BUFFER_COUNT = 2; //スワップチェーンのバッファ数

public:	//公開関数
	Engine();	//コンストラクタ
	~Engine();	//デストラクタ

	//メイン処理関数
	bool Initialize(			//初期化
		HWND hwnd,					//ウィンドウハンドル
		UINT m_FrameBufferWidth,	//フレームバッファの幅
		UINT m_FrameBufferHeight	//フレームバッファの高さ
	);
	void Terminate();			//終了

	//描画関連関数
	void RenderBegin();	//描画開始
	void WaitRender();	//前のフレームの終了待ち
	void RenderEnd();	//描画終了

	//各種ゲッター
	ID3D12Device* GetDevice() { return m_pDevice.Get(); }							//デバイスの取得
	ID3D12GraphicsCommandList* GetCommandList() { return m_pCommandList.Get(); }	//コマンドリストの取得
	UINT GetCurrentBufferIndex() const { return m_currentBackBufferIndex; }			//フレームバッファのインデックスの取得

private:
	//ウィンドウ関連
	HWND hwnd = nullptr;		//ウィンドウハンドル

private:	//DirectX12関連
	ComPtr<ID3D12Device> m_pDevice;												//デバイス
	ComPtr<ID3D12CommandAllocator> m_pCommandAllocator[FRAME_BUFFER_COUNT];		//コマンドアロケータ
	ComPtr<ID3D12GraphicsCommandList> m_pCommandList;							//コマンドリスト
	ComPtr<ID3D12CommandQueue> m_pCommandQueue;									//コマンドキュー
	ComPtr<IDXGISwapChain4> m_pSwapChain;										//スワップチェーン

	HANDLE m_fenceEvent = nullptr;				//フェンスイベントハンドル
	ComPtr<ID3D12Fence> m_pFence;				//フェンス
	UINT64 m_fenceValue[FRAME_BUFFER_COUNT]{};	//フェンス値

	D3D12_VIEWPORT m_viewport{};	//ビューポート
	D3D12_RECT m_scissorRect{};		//シザー矩形

private:	//描画関連
	UINT m_FrameBufferWidth = 0;		//フレームバッファの幅
	UINT m_FrameBufferHeight = 0;		//フレームバッファの高さ
	UINT m_currentBackBufferIndex = 0;	//現在のバックバッファインデックス

	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap;									//RTV用デスクリプタヒープ
	UINT m_rtvDescriptorSize = 0;												//RTVデスクリプタサイズ
	ComPtr<ID3D12Resource> m_pRenderTargets[FRAME_BUFFER_COUNT] = { nullptr };	//レンダーターゲット（ダブルバッファリングするので2個）

	UINT m_dsvDescriptorSize = 0;							//深度ステンシルのディスクリプターサイズ
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr;		//深度ステンシルのディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr;	//深度ステンシルバッファ（こっちは1つでいい）

	ID3D12Resource* m_currentRenderTarget = nullptr; // 現在のフレームのレンダーターゲットを一時的に保存しておく変数

private:	//結果コード
	HRESULT result = S_OK;	//HRESULT(成功/失敗コード)

private:	//内部関数
	//各種生成関数
	void CreateDevice();			//デバイスの生成
	void CreateCommandObjects();	//コマンドオブジェクトの生成
	void CreateSwapChain();			//スワップチェーンの生成
	void CreateFence();				//フェンスの生成
	void CreateViewport();			//ビューポートの生成
	void CreateScissorRect();		//シザー矩形の生成
	void CreateRenderTarget();		//レンダーターゲットの生成
	void CreateDepthStencil();		//深度ステンシルの生成
};