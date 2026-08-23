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

class TextureManager;

// Render target types
enum class RENDER_TARGET_TYPE
{
	BACK_BUFFER_0 = 0,	// Back buffer render target 0
	BACK_BUFFER_1,		// Back buffer render target 1
	POST_PROCESS,		// Post-processing render target
	TYPE_COUNT,			// Number of types
};

// Render target slot structure
struct RenderTargetSlot
{
	ComPtr<ID3D12Resource> renderTarget = { nullptr };	// Render targets(Back buffer + post-processing)
	uint32_t rtvIndex = 0;								// RTV descriptor index (for back buffer, it is the same as the back buffer index; for post-processing, it is a fixed index)
	D3D12_RESOURCE_STATES m_currenttargetState{};		// Render target states(Back buffer + post-processing)
	float clearColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };	// Clear color (RGBA)
};

//DirectX12エンジン
class Engine
{
public:
	static constexpr int FRAME_BUFFER_COUNT = 2; //スワップチェーンのバッファ数

public:	//公開関数
	Engine();	//コンストラクタ
	~Engine();	//デストラクタ

	//メイン処理関数
	bool InitCore(			//初期化
		HWND hwnd,					//ウィンドウハンドル
		UINT m_FrameBufferWidth,	//フレームバッファの幅
		UINT m_FrameBufferHeight	//フレームバッファの高さ
	);
	bool InitBindings(TextureManager* pTextureManager);	// Initialize bindings (root signature, descriptor heaps, etc.)
	void Terminate();			//終了

	// Rendering related functions
	void BeginPass(RENDER_TARGET_TYPE type);	// Set up render target
	void EndPass(RENDER_TARGET_TYPE type);		// End render pass
	void BeginFrame();							// Start rendering
	bool WaitRender();							// Wait for the previous frame to finish
	bool RenderEnd();							// End rendering

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

	// Render target related
	RenderTargetSlot m_renderTargetSlots[static_cast<int>(RENDER_TARGET_TYPE::TYPE_COUNT)] = {};	// Render target slots (back buffer + post-processing)
	ComPtr<ID3D12DescriptorHeap> m_pRTVHeap = nullptr;												// Current frame's RTV descriptor heap (temporarily stored)
	uint32_t m_rtvDescriptorSize = 0;																// RTV descriptor size (temporarily stored)

	// Depth stencil related
	UINT m_dsvDescriptorSize = 0;							//深度ステンシルのディスクリプターサイズ
	ComPtr<ID3D12DescriptorHeap> m_pDsvHeap = nullptr;		//深度ステンシルのディスクリプタヒープ
	ComPtr<ID3D12Resource> m_pDepthStencilBuffer = nullptr;	//深度ステンシルバッファ（こっちは1つでいい）

	TextureManager* m_pTextureManager = nullptr;	// Texture manager (for post-processing render target)

private:	//結果コード
	HRESULT result = S_OK;	//HRESULT(成功/失敗コード)

private:	//内部関数
	//各種生成関数
	bool CreateDevice();					// Device creation
	bool CreateCommandObjects();			// Command object creation
	bool CreateSwapChain();					// Swap chain creation
	bool CreateFence();						// Fence creation
	void CreateViewport();					// Viewport creation
	void CreateScissorRect();				// Scissor rectangle creation
	bool CreateRTVHeap();					// RTV descriptor heap creation
	bool CreateRenderTarget();				// Render target creation
	bool CreatePostProcessRenderTarget();	// Post-processing render target creation
	bool CreateDepthStencil();				// Depth stencil creation

	RenderTargetSlot& GetRenderTargetSlot(RENDER_TARGET_TYPE type);
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVHandle(uint32_t idx);
};
