#include "ShaderLibrary.h"
#include <cassert>

// Output compile error messages
static inline void OutputCompileError(ID3DBlob* errorBlob, const wchar_t* file)
{
	if (!errorBlob) return;

	const char* msg = static_cast<const char*>(errorBlob->GetBufferPointer());
	if (!msg) return;

	OutputDebugStringW(L"[ShaderLibrary] Compile failed: ");
	OutputDebugStringW(file);
	OutputDebugStringW(L"\n");

	OutputDebugStringA(msg);
	OutputDebugStringA("\n");
}

// Constructor
ShaderLibrary::ShaderLibrary()
{
	m_shaderCache.clear();
}

// Get vertex shader
Microsoft::WRL::ComPtr<ID3DBlob> ShaderLibrary::GetVS(VS_ID vsId, uint64_t defines)
{
	// Validate vsId
	const size_t index = static_cast<size_t>(vsId);
	if(index >= sizeof(VS_TABLE) / sizeof(VS_TABLE[0]))
	{
		OutputDebugStringA("[ShaderLibrary] Invalid VS_ID\n");
		return {};
	}

	// Get or compile shader
	return GetOrCompileShader(VS_TABLE[index], defines);
}

// Get pixel shader
Microsoft::WRL::ComPtr<ID3DBlob> ShaderLibrary::GetPS(PS_ID psId, uint64_t defines)
{
	// Validate psId
	const size_t index = static_cast<size_t>(psId);
	if (index >= sizeof(PS_TABLE) / sizeof(PS_TABLE[0]))
	{
		OutputDebugStringA("[ShaderLibrary] Invalid PS_ID\n");
		return {};
	}

	// Get or compile shader
	return GetOrCompileShader(PS_TABLE[index], defines);
}

// Get or compile shader
Microsoft::WRL::ComPtr<ID3DBlob> ShaderLibrary::GetOrCompileShader(const ShaderDesc& desc, uint64_t defines)
{
	// Create shader key
	ShaderKey key = std::tie(
		desc.filePath,
		desc.entryPoint,
		desc.profile,
		defines
	);

	// Check if shader is already cached
	auto it = m_shaderCache.find(key);
	if (it != m_shaderCache.end())
	{// Return cached shader
		return it->second;
	}

	// Compile shader
	Microsoft::WRL::ComPtr<ID3DBlob> shaderBlob;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob;

	UINT flags = 0;
#ifdef _DEBUG
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif
	flags |= D3DCOMPILE_ENABLE_STRICTNESS;

	HRESULT hr = D3DCompileFromFile(
		desc.filePath.c_str(),
		nullptr,
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		desc.entryPoint.c_str(),
		desc.profile.c_str(),
		flags,
		0,
		shaderBlob.GetAddressOf(),
		errorBlob.GetAddressOf()
	);

	if(FAILED(hr) || !shaderBlob)
	{
		OutputCompileError(errorBlob.Get(), desc.filePath.c_str());
		return {};
	}

	// Cache the compiled shader
	m_shaderCache.emplace(key, shaderBlob);
	return shaderBlob;
}
