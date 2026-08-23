#pragma once
#include <d3d12.h>
#include "DirectXTex.h"
#include <unordered_map>
#include <string>
#include <vector>
#include <memory>
#include "ComPtr.h"

//アップロード待ちテクスチャ構造体
struct PendingTextureUpload
{
	ComPtr<ID3D12Resource> texture;						//テクスチャリソース
	ComPtr<ID3D12Resource> uploadBuffer;				//アップロード用バッファ
	std::unique_ptr<DirectX::ScratchImage> image;		//スクラッチイメージ
	uint32_t srvIndex;									//SRVディスクリプタインデックス
};

// Reserved SRV indices for special textures
enum class TEXTURE_SRV_INDEX_RESERVED : uint32_t
{
	POST_PROCESSING = 0,	// Reserved index for post-processing texture
	DEFAULT_TEXTURE,		// Reserved index for default white texture
	RESERVED_COUNT			// Count of reserved indices
};

// テクスチャマネージャークラス
class TextureManager
{
public:
	TextureManager() {};	//コンストラクタ
	~TextureManager() {};	//デストラクタ

	//コピー禁止
	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	//ムーブセマンティクス許可
	TextureManager(TextureManager&&) noexcept = default;
	TextureManager& operator=(TextureManager&&) noexcept = default;

	bool Initialize(	//初期化
		ID3D12Device* pDevice,	//デバイス
		uint32_t maxDescriptors	//最大ディスクリプタ数
	);

	uint32_t LoadSrvFromFile(	//ファイルからSRVを読み込み
		const std::wstring& path	//ファイルパス
	);

	uint32_t AllocateSrv();	// Allocate SRV descriptor index (for manually created textures)
	void CreateSrv(			// Create SRV for a texture resource
		ID3D12Resource* pResource,	// Texture resource
		DXGI_FORMAT format,			// Texture format
		uint32_t srvIndex			// SRV descriptor index
	);

	void UploadPendingTextures(ID3D12GraphicsCommandList* cmdList);	//アップロード待ちテクスチャをアップロード
	void ReleaseCompletedUploads();	// GPU copy completion must be guaranteed by the caller.

	ID3D12DescriptorHeap* GetSrvHeap() const;	//SRVヒープを取得(ここにSRVが格納されている)
	UINT GetSrvIncrementSize() const;			//SRVディスクリプタのインクリメントサイズを取得

	uint32_t GetPostProcessingTextureIndex() const;	// Get post-processing texture index
	uint32_t GetDefaultTextureIndex() const;		// Get default texture index
private:
	// SRV management
	ComPtr<ID3D12DescriptorHeap> m_pSrvHeap;												// SRV descriptor heap (where texture SRVs are stored)
	UINT m_srvIncrementSize = 0;															//SRVディスクリプタのインクリメントサイズ
	UINT m_nextFreeIndex = static_cast<UINT>(TEXTURE_SRV_INDEX_RESERVED::RESERVED_COUNT);	//次に使用可能なディスクリプタインデックス

	std::unordered_map<std::wstring, uint32_t> m_loadedTextures;	//読み込まれたテクスチャのマップ
	std::vector<ComPtr<ID3D12Resource>> m_textures;					//テクスチャリソース配列
	std::vector< ComPtr<ID3D12Resource> > m_uploadKeepAlive;		//アップロード用バッファのキープアライブ配列
	std::vector<PendingTextureUpload> m_pendingUploads;				//アップロード待ちテクスチャ配列

	ID3D12Device* m_pDevice = nullptr;	//デバイス

private:
	bool CreateDefaultTexture();	// Create default texture
};
