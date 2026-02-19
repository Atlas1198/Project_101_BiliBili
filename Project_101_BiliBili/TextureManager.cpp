#include "TextureManager.h"
#include "d3dx12.h"
#include "DirectXTex.h"
#include <filesystem>

using namespace DirectX;

//ファイルの拡張子を取得
std::wstring FileExtension(const std::wstring& path)
{
	std::filesystem::path fsPath(path);				//ファイルパスからfilesystemパスを作成
	return fsPath.extension().wstring().substr(1); //拡張子の先頭のドットを除去して返す
}

//初期化
void TextureManager::Initialize(
	ID3D12Device* pDevice,	//デバイス
	uint32_t maxDescriptors	//最大ディスクリプタ数
)
{
	m_pDevice = pDevice;	//デバイスを保存

	//SRVディスクリプタヒープの設定
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};	//ディスクリプタヒープ記述子
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		//SRVヒープ
	desc.NumDescriptors = maxDescriptors;					//ディスクリプタ数
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; //シェーダーから見える

	//ディスクリプタヒープの生成
	pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pSrvHeap));

	//SRVディスクリプタのインクリメントサイズを取得
	m_srvIncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_nextFreeIndex = 0;		//次の空きインデックスを初期化
	m_loadedTextures.clear();	//読み込み済みテクスチャマップをクリア

	//デフォルトの白テクスチャを作成
	m_defaultTextureIndex = LoadSrvFromFile(L"asset/texture/white.png");
}

//ファイルからSRVを読み込み
uint32_t TextureManager::LoadSrvFromFile(const std::wstring& path)
{
	//すでに読み込まれている場合はそのインデックスを返す
	if (auto it = m_loadedTextures.find(path); it != m_loadedTextures.end())
	{
		return it->second;
	}

	//画像の読み込み
	ScratchImage img = {};	//スクラッチイメージ
	TexMetadata meta = {};		//メタデータ

	auto ext = FileExtension(path);	//拡張子取得

	HRESULT hr = S_FALSE;	//HRESULT

	//拡張子で読み込み方法を分岐
	if (ext == L"tga")
	{//TGAファイルから読み込み
		hr = LoadFromTGAFile(path.c_str(), &meta, img);
	}
	else
	{//WICファイルから読み込み
		hr = LoadFromWICFile(		//WICファイルから読み込み
			path.c_str(),	//ファイルパス
			WIC_FLAGS_NONE,	//WICフラグ
			&meta,			//メタデータ
			img				//スクラッチイメージ
		);
	}

	//読み込み失敗の場合はエラーメッセージを出力して終了
	if (FAILED(hr))
	{
		OutputDebugStringW((L"[TextureManager] Failed to load: " + path + L"\n").c_str());
		return UINT32_MAX; // or some fallback texture index
	}

	//イメージデータの取得
	size_t imageCount = img.GetImageCount();	//イメージ数取得

	//イメージデータが無い場合はエラーメッセージを出力して終了
	if (imageCount == 0)
	{
		OutputDebugStringW((L"[TextureManager] No image data: " + path + L"\n").c_str());
		return UINT32_MAX;
	}

	//画像フォーマットを固定
	DXGI_FORMAT target = DXGI_FORMAT_R8G8B8A8_UNORM;
	if (meta.format != target) {
		ScratchImage conv;
		HRESULT hr2 = Convert(
			*img.GetImage(0, 0, 0),
			target,
			TEX_FILTER_DEFAULT,
			TEX_THRESHOLD_DEFAULT,
			conv
		);
		if (FAILED(hr2)) { /* error */ }

		img = std::move(conv);
		meta = img.GetMetadata(); // format がR8G8B8A8_UNORMに揃う
	}

	imageCount = img.GetImageCount();

	//GPU用テクスチャリソースの生成
	ComPtr<ID3D12Resource> pTexture;	//テクスチャリソース
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(	//テクスチャリソース記述子
		DXGI_FORMAT_R8G8B8A8_TYPELESS,						//フォーマット
		static_cast<UINT>(meta.width),		//幅
		static_cast<UINT>(meta.height),		//高さ
		1,									//配列サイズ
		static_cast<UINT>(meta.mipLevels)	//ミップレベル数
	);

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT); //ヒープ設定(デフォルト)
	hr =  m_pDevice->CreateCommittedResource(	//リソース生成
		&heapProps,						//ヒープ設定
		D3D12_HEAP_FLAG_NONE,			//ヒープフラグ
		&texDesc,						//リソース記述子
		D3D12_RESOURCE_STATE_COPY_DEST,	//初期リソース状態
		nullptr,						//最適化されたクリア値
		IID_PPV_ARGS(&pTexture)			//生成するリソース
	);

	//アップロード用バッファの生成
	ComPtr<ID3D12Resource> pUploadBuffer;	//アップロード用バッファ
	const UINT numSubresources = static_cast<UINT>(imageCount); //サブリソース数取得
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(	//アップロード用バッファサイズ取得
		pTexture.Get(),	//テクスチャリソース
		0,				//最初のサブリソース
		numSubresources	//ミップレベル数
	);

	auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); //ヒープ設定(アップロード用)
	auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize); //バッファリソース記述子

	m_pDevice->CreateCommittedResource(	//リソース生成
		&uploadHeapProps,					//ヒープ設定
		D3D12_HEAP_FLAG_NONE,				//ヒープフラグ
		&uploadBufferDesc,					//リソース記述子
		D3D12_RESOURCE_STATE_GENERIC_READ,	//初期リソース状態
		nullptr,							//最適化されたクリア値
		IID_PPV_ARGS(&pUploadBuffer)		//生成するリソース
	);

	//サブリソースデータの設定
	const uint32_t textureIndex = m_nextFreeIndex;		//次のテクスチャインデックスを取得
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;	//サブリソース配列
	subresources.reserve(img.GetImageCount());			//配列の予約
	for (size_t i = 0; i < img.GetImageCount(); ++i)
	{
		const Image* imgData = img.GetImages() + i; //イメージデータ取得

		D3D12_SUBRESOURCE_DATA subresource = {};	//サブリソースデータ
		subresource.pData = imgData->pixels;			//ピクセルデータ
		subresource.RowPitch = imgData->rowPitch;		//1行のバイト数
		subresource.SlicePitch = imgData->slicePitch;	//スライスのバイト数
		subresources.push_back(subresource);			//配列に追加
	}

	//シェーダーリソースビューの作成
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};	//SRV記述子
	srvDesc.Shader4ComponentMapping = 
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	//コンポーネントマッピング
	srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	srvDesc.ViewDimension =
		D3D12_SRV_DIMENSION_TEXTURE2D;				//ビューの次元(2Dテクスチャ)
	srvDesc.Texture2D.MipLevels = 
		(UINT)meta.mipLevels;						//ミップレベル数

	auto cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(	//CPUディスクリプタハンドル
		m_pSrvHeap->GetCPUDescriptorHandleForHeapStart(),	//ヒープ先頭ハンドル
		textureIndex,										//オフセット(読み込み済みテクスチャ数)
		m_srvIncrementSize									//インクリメントサイズ
	);

	m_pDevice->CreateShaderResourceView(	//SRV作成
		pTexture.Get(),	//テクスチャリソース
		&srvDesc,		//SRV記述子
		cpuHandle		//SRVハンドル
	);

	//テクスチャ情報を登録
	m_loadedTextures[path] = textureIndex;			//パスとインデックスを登録
	m_nextFreeIndex++;								//次の空きインデックスを更新

	//リソースとアップロード用バッファをキープ
	m_textures.push_back(pTexture);				//テクスチャリソースをキープ
	m_uploadKeepAlive.push_back(pUploadBuffer); //アップロード用バッファをキープ

	//ScratchImageをムーブして保持するためにコピー
	auto scratch = std::make_unique<ScratchImage>(std::move(img));

	//アップロード待ちテクスチャとして登録
	PendingTextureUpload pending = {};	//アップロード待ちテクスチャ
	pending.texture = pTexture;						//テクスチャリソース
	pending.uploadBuffer = pUploadBuffer;			//アップロード用バッファ
	pending.image = std::move(scratch);				//スクラッチイメージ
	pending.srvIndex = textureIndex;				//SRVディスクリプタインデックス
	m_pendingUploads.push_back(std::move(pending)); //配列に追加

	return textureIndex;	//テクスチャインデックスを返す
}

//保留中のテクスチャをアップロード
void TextureManager::UploadPendingTextures(ID3D12GraphicsCommandList* cmdList)
{
	//コマンドリストが無効、またはアップロード待ちテクスチャが無い場合は何もしない
	if (!cmdList || m_pendingUploads.empty())
	{
		return;
	}

	//保留中の各テクスチャをアップロード
	for(auto& pending : m_pendingUploads)
	{
		if (!pending.texture || !pending.uploadBuffer || !pending.image)
		{
			OutputDebugStringA("PendingTextureUpload has null member\n");
			continue;
		}

		//スクラッチイメージからイメージデータを取得
		const ScratchImage& img = *pending.image;	//スクラッチイメージ
		const Image* images = img.GetImages();		//イメージデータ取得
		size_t count = img.GetImageCount();			//イメージ数取得

		if(images == nullptr || count == 0)
		{
			continue; //無効なイメージデータはスキップ
		}

		//ここで一時的な配列を作る
		std::vector<D3D12_SUBRESOURCE_DATA> subresources(count);

		for (size_t i = 0; i < count; ++i)
		{
			D3D12_SUBRESOURCE_DATA s{};
			s.pData = images[i].pixels;
			s.RowPitch = images[i].rowPitch;
			s.SlicePitch = images[i].slicePitch;
			subresources[i] = s;
		}

		//アップロード用バッファにデータ転送
		UpdateSubresources(
			cmdList,					//コマンドリスト
			pending.texture.Get(),		//転送先リソース
			pending.uploadBuffer.Get(),	//転送元リソース
			0,							//転送元オフセット
			0,							//最初のサブリソース
			static_cast<UINT>(count),	//サブリソース数
			subresources.data()			//サブリソース配列
		);

		//テクスチャの状態をコピー先からピクセルシェーダーリソースへ変更
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pending.texture.Get(),						//リソース
			D3D12_RESOURCE_STATE_COPY_DEST,				//変化前の状態
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	//変化後の状態
		);
		cmdList->ResourceBarrier(1, &barrier);	//リソースバリア設定
	}

	//アップロード待ちテクスチャ配列をクリア
	m_pendingUploads.clear();
}

//SRVヒープを取得
ID3D12DescriptorHeap* TextureManager::GetSrvHeap() const
{
	return m_pSrvHeap.Get();
}

//SRVディスクリプタのインクリメントサイズを取得
UINT TextureManager::GetSrvIncrementSize() const
{
	return m_srvIncrementSize;
}

//デフォルトの白テクスチャのSRVインデックスを取得
uint32_t TextureManager::GetDefaultWhiteTextureIndex() const
{
	return m_defaultTextureIndex;
}
