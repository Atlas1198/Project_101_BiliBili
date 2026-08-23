#include "TextureManager.h"
#include "d3dx12.h"
#include <filesystem>
#include <DirectXTex.h>
#include <cstring>

using namespace DirectX;

namespace
{
	constexpr size_t kMaxRuntimeTextureDimension = 4096;
}

// Get file extension from path
std::wstring FileExtension(const std::wstring& path)
{
	std::filesystem::path fsPath(path);				// Create filesystem path from file path
	return fsPath.extension().wstring().substr(1); // Remove leading dot from extension and return
}

// Initialization
bool TextureManager::Initialize(
	ID3D12Device* pDevice,	// Device
	uint32_t maxDescriptors	// Maximum descriptor count
)
{
	if (!pDevice || maxDescriptors <= static_cast<uint32_t>(TEXTURE_SRV_INDEX_RESERVED::RESERVED_COUNT))
	{
		OutputDebugStringA("[TextureManager] Invalid initialization parameters\n");
		return false;
	}

	m_pDevice = pDevice;	// Save device
	m_pSrvHeap.Reset();
	m_textures.clear();
	m_uploadKeepAlive.clear();
	m_pendingUploads.clear();

	// SRV descriptor heap setup
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};	// Descriptor heap descriptor
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;		// SRV heap
	desc.NumDescriptors = maxDescriptors;					// Descriptor count
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE; // Shader visible

	// Create descriptor heap
	const HRESULT heapResult = pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_pSrvHeap));
	if (FAILED(heapResult))
	{
		OutputDebugStringA("[TextureManager] Failed to create SRV descriptor heap\n");
		return false;
	}

	// Get SRV descriptor increment size
	m_srvIncrementSize = pDevice->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	m_nextFreeIndex = static_cast<uint32_t>(TEXTURE_SRV_INDEX_RESERVED::RESERVED_COUNT);	// Initialize next free index
	m_loadedTextures.clear();																// Clear loaded texture map

	// Load default white texture
	return CreateDefaultTexture();
}

// Load SRV from file
uint32_t TextureManager::LoadSrvFromFile(const std::wstring& path)
{
	const uint32_t fallbackIndex = GetDefaultTextureIndex();
	if (!m_pDevice || !m_pSrvHeap)
	{
		OutputDebugStringA("[TextureManager] Load requested before initialization\n");
		return fallbackIndex;
	}

	// If already loaded, return the index
	if (auto it = m_loadedTextures.find(path); it != m_loadedTextures.end())
	{
		return it->second;
	}

	// Load image
	ScratchImage img = {};	// Scratch image
	TexMetadata meta = {};		// Metadata

	auto ext = FileExtension(path);	// Get file extension

	HRESULT hr = S_FALSE;	// HRESULT

	// Switch loading method based on extension
	if (ext == L"tga")
	{// Load from TGA file
		hr = LoadFromTGAFile(path.c_str(), &meta, img);
	}
	else
	{// Load from WIC file
		hr = LoadFromWICFile(		// Load from WIC file
			path.c_str(),	// File path
			WIC_FLAGS_NONE,	// WIC flags
			&meta,			// Metadata
			img				// Scratch image
		);
	}

	// If loading failed, output error message and return
	if (FAILED(hr))
	{
		OutputDebugStringW((L"[TextureManager] Failed to load: " + path + L"\n").c_str());
		return fallbackIndex;
	}

	// If no image data, output error message and return
	if (img.GetImageCount() == 0)
	{
		OutputDebugStringW((L"[TextureManager] No image data: " + path + L"\n").c_str());
		return fallbackIndex;
	}

	if (meta.width == 0 || meta.height == 0)
	{
		OutputDebugStringW((L"[TextureManager] Unsupported texture dimensions: " + path + L"\n").c_str());
		return fallbackIndex;
	}

	// Keep unexpectedly large source assets from consuming hundreds of MiB of VRAM.
	// The source file is left untouched; production assets should still be resized offline.
	if (meta.width > kMaxRuntimeTextureDimension || meta.height > kMaxRuntimeTextureDimension)
	{
		const double scale = std::min(
			static_cast<double>(kMaxRuntimeTextureDimension) / static_cast<double>(meta.width),
			static_cast<double>(kMaxRuntimeTextureDimension) / static_cast<double>(meta.height));
		const size_t resizedWidth = std::max<size_t>(1, static_cast<size_t>(meta.width * scale));
		const size_t resizedHeight = std::max<size_t>(1, static_cast<size_t>(meta.height * scale));

		ScratchImage resizedImage;
		hr = Resize(
			img.GetImages(),
			img.GetImageCount(),
			meta,
			resizedWidth,
			resizedHeight,
			TEX_FILTER_DEFAULT,
			resizedImage);
		if (FAILED(hr))
		{
			OutputDebugStringW((L"[TextureManager] Failed to resize oversized texture: " + path + L"\n").c_str());
			return fallbackIndex;
		}

		img = std::move(resizedImage);
		meta = img.GetMetadata();
		OutputDebugStringW((L"[TextureManager] Resized oversized texture for runtime use: " + path + L"\n").c_str());
	}

	const size_t imageCount = img.GetImageCount();

	// Create texture resource
	ComPtr<ID3D12Resource> pTexture;	// Texture resource
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(	// Texture resource descriptor
		meta.format,						// Format
		static_cast<UINT>(meta.width),		// Width
		static_cast<UINT>(meta.height),		// Height
		1,									// Array size
		static_cast<UINT>(meta.mipLevels)	// Mip levels
	);

	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT); // Heap properties (default)
	hr = m_pDevice->CreateCommittedResource(	// Create resource
		&heapProps,						// Heap properties
		D3D12_HEAP_FLAG_NONE,			// Heap flags
		&texDesc,						// Resource descriptor
		D3D12_RESOURCE_STATE_COPY_DEST,	// Initial resource state
		nullptr,						// Optimized clear value
		IID_PPV_ARGS(&pTexture)			// Resource to create
	);
	if (FAILED(hr) || !pTexture)
	{
		OutputDebugStringW((L"[TextureManager] Failed to create texture resource: " + path + L"\n").c_str());
		return fallbackIndex;
	}

	// Create upload buffer
	ComPtr<ID3D12Resource> pUploadBuffer;	// Upload buffer
	const UINT numSubresources = static_cast<UINT>(imageCount); // Get subresource count
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize(	// Get upload buffer size
		pTexture.Get(),	// Texture resource
		0,				// First subresource
		numSubresources	// Mip levels
	);

	auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // Heap properties (upload)
	auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize); // Buffer resource descriptor

	hr = m_pDevice->CreateCommittedResource(	// Create resource
		&uploadHeapProps,					// Heap properties
		D3D12_HEAP_FLAG_NONE,				// Heap flags
		&uploadBufferDesc,					// Resource descriptor
		D3D12_RESOURCE_STATE_GENERIC_READ,	// Initial resource state
		nullptr,							// Optimized clear value
		IID_PPV_ARGS(&pUploadBuffer)		// Resource to create
	);
	if (FAILED(hr) || !pUploadBuffer)
	{
		OutputDebugStringW((L"[TextureManager] Failed to create upload buffer: " + path + L"\n").c_str());
		return fallbackIndex;
	}

	// Set subresource data
	const uint32_t textureIndex = AllocateSrv();		// Get next texture index
	if (textureIndex == UINT32_MAX)
	{
		return fallbackIndex;
	}
	std::vector<D3D12_SUBRESOURCE_DATA> subresources;	// Subresource array
	subresources.reserve(img.GetImageCount());			// Reserve array size
	for (size_t i = 0; i < img.GetImageCount(); ++i)
	{
		const Image* imgData = img.GetImages() + i; // Get image data

		D3D12_SUBRESOURCE_DATA subresource = {};	// Subresource data
		subresource.pData = imgData->pixels;			// Pixel data
		subresource.RowPitch = imgData->rowPitch;		// Row pitch
		subresource.SlicePitch = imgData->slicePitch;	// Slice pitch
		subresources.push_back(subresource);			// Add to array
	}

	// Create shader resource view
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};	// SRV descriptor
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// Component mapping
	srvDesc.Format =
		meta.format;								// Format
	srvDesc.ViewDimension =
		D3D12_SRV_DIMENSION_TEXTURE2D;				// View dimension (2D texture)
	srvDesc.Texture2D.MipLevels =
		(UINT)meta.mipLevels;						// Mip levels

	auto cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(	// CPU descriptor handle
		m_pSrvHeap->GetCPUDescriptorHandleForHeapStart(),	// Heap start handle
		textureIndex,										// Offset (number of loaded textures)
		m_srvIncrementSize									// Increment size
	);

	m_pDevice->CreateShaderResourceView(	// Create SRV
		pTexture.Get(),	// Texture resource
		&srvDesc,		// SRV descriptor
		cpuHandle		// SRV handle
	);

	// Register texture information
	m_loadedTextures[path] = textureIndex;			// Register path and index

	// Keep resources and upload buffer alive
	m_textures.push_back(pTexture);				// Keep texture resource
	m_uploadKeepAlive.push_back(pUploadBuffer); // Keep upload buffer

	// Copy to keep ScratchImage alive

	auto scratch = std::make_unique<ScratchImage>(std::move(img));

	// Register as pending texture upload
	PendingTextureUpload pending = {};	// Pending texture upload
	pending.texture = pTexture;						// Texture resource
	pending.uploadBuffer = pUploadBuffer;			// Upload buffer
	pending.image = std::move(scratch);				// Scratch image
	pending.srvIndex = textureIndex;				// SRV descriptor index
	m_pendingUploads.push_back(std::move(pending)); // Add to array

	return textureIndex;	// Return texture index
}

// Allocate SRV descriptor index (for manually created textures)
uint32_t TextureManager::AllocateSrv()
{
	if (m_nextFreeIndex >= m_pSrvHeap->GetDesc().NumDescriptors)
	{
		OutputDebugStringA("[TextureManager] No more SRV descriptors available\n");
		return UINT32_MAX; // or some fallback texture index
	}

	return m_nextFreeIndex++;
}

// Create SRV for a texture resource
void TextureManager::CreateSrv(
	ID3D12Resource* pResource,						// Texture resource
	DXGI_FORMAT format,								// Texture format
	uint32_t srvIndex								// SRV descriptor index
)
{
	if (!pResource)
	{
		OutputDebugStringA("[TextureManager] Invalid resource for CreateSrv\n");
		return;
	}

	if (srvIndex >= m_pSrvHeap->GetDesc().NumDescriptors)
	{
		OutputDebugStringA("[TextureManager] SRV index out of bounds for CreateSrv\n");
		return;
	}

	auto cpuHandle = CD3DX12_CPU_DESCRIPTOR_HANDLE(	// CPU descriptor handle
		m_pSrvHeap->GetCPUDescriptorHandleForHeapStart(),	// Heap start handle
		srvIndex,											// Offset (SRV index)
		m_srvIncrementSize									// Increment size
	);

	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};	// SRV descriptor
	srvDesc.Shader4ComponentMapping =
		D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;	// Component mapping
	srvDesc.Format = format;						// Format
	srvDesc.ViewDimension =
		D3D12_SRV_DIMENSION_TEXTURE2D;				// View dimension (2D texture)
	srvDesc.Texture2D.MipLevels = 1;				// Mip levels (1 for manually created textures)
	srvDesc.Texture2D.MostDetailedMip = 0;			// Most detailed mip (0 for manually created textures)
	srvDesc.Texture2D.ResourceMinLODClamp = 0.0f;	// Resource min LOD clamp

	m_pDevice->CreateShaderResourceView(	// Create SRV
		pResource,		// Texture resource
		&srvDesc,		// SRV descriptor
		cpuHandle		// SRV handle
	);
}

// Upload pending textures
void TextureManager::UploadPendingTextures(ID3D12GraphicsCommandList* cmdList)
{
	// If command list is invalid or no pending texture uploads, do nothing
	if (!cmdList || m_pendingUploads.empty())
	{
		return;
	}

	// Upload each pending texture
	for (auto& pending : m_pendingUploads)
	{
		if (!pending.texture || !pending.uploadBuffer || !pending.image)
		{
			OutputDebugStringA("PendingTextureUpload has null member\n");
			continue;
		}

		// Get image data from ScratchImage
		const ScratchImage& img = *pending.image;	// ScratchImage
		const Image* images = img.GetImages();		// Get image data
		size_t count = img.GetImageCount();			// Get image count

		if (images == nullptr || count == 0)
		{
			continue; // Skip invalid image data
		}

		// Create temporary array here
		std::vector<D3D12_SUBRESOURCE_DATA> subresources(count);

		for (size_t i = 0; i < count; ++i)
		{
			D3D12_SUBRESOURCE_DATA s{};
			s.pData = images[i].pixels;
			s.RowPitch = images[i].rowPitch;
			s.SlicePitch = images[i].slicePitch;
			subresources[i] = s;
		}

		// Upload data to upload buffer
		UpdateSubresources(
			cmdList,					// Command list
			pending.texture.Get(),		// Destination resource
			pending.uploadBuffer.Get(),	// Source resource
			0,							// Source offset
			0,							// First subresource
			static_cast<UINT>(count),	// Number of subresources
			subresources.data()			// Subresource array
		);

		// Change texture state from copy destination to pixel shader resource
		auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
			pending.texture.Get(),						// Resource
			D3D12_RESOURCE_STATE_COPY_DEST,				// Before state
			D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE	// After state
		);
		cmdList->ResourceBarrier(1, &barrier);	// Set resource barrier
	}

	// Clear pending texture upload array
	m_pendingUploads.clear();
}

void TextureManager::ReleaseCompletedUploads()
{
	// Upload resources must stay alive until the command queue has completed the copy.
	// App calls this only after Engine::RenderEnd has successfully waited on its fence.
	m_uploadKeepAlive.clear();
}

// Get SRV heap
ID3D12DescriptorHeap* TextureManager::GetSrvHeap() const
{
	return m_pSrvHeap.Get();
}

// Get SRV descriptor increment size
UINT TextureManager::GetSrvIncrementSize() const
{
	return m_srvIncrementSize;
}

// Get post-processing texture index
uint32_t TextureManager::GetPostProcessingTextureIndex() const
{
	return static_cast<uint32_t>(TEXTURE_SRV_INDEX_RESERVED::POST_PROCESSING);
}

// Get default white texture index
uint32_t TextureManager::GetDefaultTextureIndex() const
{
	return static_cast<uint32_t>(TEXTURE_SRV_INDEX_RESERVED::DEFAULT_TEXTURE);
}

// Create default texture
bool TextureManager::CreateDefaultTexture()
{
	// Create a 1x1 white texture
	const uint32_t defaultTextureIndex = static_cast<uint32_t>(TEXTURE_SRV_INDEX_RESERVED::DEFAULT_TEXTURE); // Default white texture index
	// Create texture resource
	ComPtr<ID3D12Resource> pTexture;	// Texture resource
	CD3DX12_RESOURCE_DESC texDesc = CD3DX12_RESOURCE_DESC::Tex2D(	// Texture resource descriptor
		DXGI_FORMAT_R8G8B8A8_UNORM,			// Format (RGBA8)
		1,									// Width
		1,									// Height
		1,									// Array size
		1									// Mip levels
	);
	auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT); // Heap properties (default)
	HRESULT hr = m_pDevice->CreateCommittedResource(	// Create resource
		&heapProps,						// Heap properties
		D3D12_HEAP_FLAG_NONE,			// Heap flags
		&texDesc,						// Resource descriptor
		D3D12_RESOURCE_STATE_COPY_DEST, // Initial resource state
		nullptr,						// Optimized clear value
		IID_PPV_ARGS(&pTexture)			// Resource to create
	);
	if (FAILED(hr) || !pTexture)
	{
		OutputDebugStringA("[TextureManager] Failed to create default texture\n");
		return false;
	}

	ScratchImage defaultImage;
	hr = defaultImage.Initialize2D(DXGI_FORMAT_R8G8B8A8_UNORM, 1, 1, 1, 1);
	if (FAILED(hr) || !defaultImage.GetPixels())
	{
		OutputDebugStringA("[TextureManager] Failed to initialize default texture pixels\n");
		return false;
	}
	const uint32_t defaultPixel = 0xFFFF00FF;
	std::memcpy(defaultImage.GetPixels(), &defaultPixel, sizeof(defaultPixel));
	const UINT64 uploadBufferSize = GetRequiredIntermediateSize( // Get upload buffer size
		pTexture.Get(),				 // Texture resource
		0,							 // First subresource
		1							 // Mip levels
	);
	auto uploadHeapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD); // Heap properties (upload)
	auto uploadBufferDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize); // Buffer resource descriptor
	ComPtr<ID3D12Resource> uploadBuffer;
	hr = m_pDevice->CreateCommittedResource( // Create resource
		&uploadHeapProps,				 // Heap properties
		D3D12_HEAP_FLAG_NONE,			 // Heap flags
		&uploadBufferDesc,				 // Resource descriptor
		D3D12_RESOURCE_STATE_GENERIC_READ, // Initial resource state
		nullptr,						 // Optimized clear value
		IID_PPV_ARGS(&uploadBuffer) // Resource to create and keep alive
	);
	if (FAILED(hr) || !uploadBuffer)
	{
		OutputDebugStringA("[TextureManager] Failed to create default texture upload buffer\n");
		return false;
	}

	// Create shader resource view
	CreateSrv(
		pTexture.Get(),						// Texture resource
		DXGI_FORMAT_R8G8B8A8_UNORM,			// Format
		defaultTextureIndex						// SRV descriptor index
	);

	m_textures.push_back(pTexture); // Keep texture resource alive
	m_uploadKeepAlive.push_back(uploadBuffer);

	PendingTextureUpload pending = {};
	pending.texture = pTexture;
	pending.uploadBuffer = uploadBuffer;
	pending.image = std::make_unique<ScratchImage>(std::move(defaultImage));
	pending.srvIndex = defaultTextureIndex;
	m_pendingUploads.push_back(std::move(pending));

	return true;
}
