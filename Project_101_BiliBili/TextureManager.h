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

	void Initialize(	//初期化
		ID3D12Device* pDevice,	//デバイス
		uint32_t maxDescriptors	//最大ディスクリプタ数
	);

	uint32_t LoadSrvFromFile(	//ファイルからSRVを読み込み	
		const std::wstring& path	//ファイルパス
	);

	void UploadPendingTextures(ID3D12GraphicsCommandList* cmdList);	//アップロード待ちテクスチャをアップロード

	ID3D12DescriptorHeap* GetSrvHeap() const;	//SRVヒープを取得(ここにSRVが格納されている)
	UINT GetSrvIncrementSize() const;			//SRVディスクリプタのインクリメントサイズを取得

private:
	ComPtr<ID3D12DescriptorHeap> m_pSrvHeap;	//SRVディスクリプタヒープ(テクスチャのSRVが格納される)

	UINT m_srvIncrementSize = 0;	//SRVディスクリプタのインクリメントサイズ
	UINT m_nextFreeIndex = 0;		//次に使用可能なディスクリプタインデックス

	std::unordered_map<std::wstring, uint32_t> m_loadedTextures;	//読み込まれたテクスチャのマップ
	std::vector<ComPtr<ID3D12Resource>> m_textures;					//テクスチャリソース配列
	std::vector< ComPtr<ID3D12Resource> > m_uploadKeepAlive;		//アップロード用バッファのキープアライブ配列
	std::vector<PendingTextureUpload> m_pendingUploads;				//アップロード待ちテクスチャ配列

	ID3D12Device* m_pDevice = nullptr;	//デバイス
};