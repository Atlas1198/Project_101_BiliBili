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

static UINT GetCompileFlags()
{
	UINT flags = D3DCOMPILE_ENABLE_STRICTNESS;

#if defined(_DEBUG)
	flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	flags |= D3DCOMPILE_OPTIMIZATION_LEVEL3;
#endif

	return flags;
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
	return GetOrCompileShader(VS_TABLE[index], defines & VS_DEFINE_MASK);
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
	return GetOrCompileShader(PS_TABLE[index], defines & PS_DEFINE_MASK);
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

	auto macros = BuildMacros(defines);

	HRESULT hr = D3DCompileFromFile(
		desc.filePath.c_str(),
		macros.data(),
		D3D_COMPILE_STANDARD_FILE_INCLUDE,
		desc.entryPoint.c_str(),
		desc.profile.c_str(),
		GetCompileFlags(),
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

std::vector<D3D_SHADER_MACRO> ShaderLibrary::BuildMacros(uint64_t defines)
{
	struct Entry {
		uint64_t bit;
		const char* name;
		const char* value;
	};

	static const Entry table[] = {
		{ static_cast<uint64_t>(SHADER_DEFINE::PS_USE_MASK), "PS_USE_MASK", "1" },
		{ static_cast<uint64_t>(SHADER_DEFINE::PS_MULTIPLY_ALPHA_CONTROL), "PS_MULTIPLY_ALPHA_CONTROL", "1" },
		{ static_cast<uint64_t>(SHADER_DEFINE::PS_USE_LIGHTING), "PS_USE_LIGHTING", "1" },
		{ static_cast<uint64_t>(SHADER_DEFINE::PS_OUTLINE_RED), "PS_OUTLINE_RED", "1" },
		{ static_cast<uint64_t>(SHADER_DEFINE::PS_OUTLINE_BLUE), "PS_OUTLINE_BLUE", "1" },
		{ static_cast<uint64_t>(SHADER_DEFINE::PS_WRAP_UV), "PS_WRAP_UV", "1" },
	};

	std::vector<D3D_SHADER_MACRO> macros;
	macros.reserve(std::size(table) + 1);

	for (auto& element : table)
	{
		if ((defines & element.bit) != 0) { 
			macros.push_back({ element.name, element.value }); 
		}
	}

	macros.push_back({ nullptr, nullptr });
	return macros;
}
