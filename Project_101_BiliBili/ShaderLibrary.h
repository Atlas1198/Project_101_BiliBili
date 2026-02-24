#pragma once
#define _WIN32_WINNT 0x0A00
#include <sdkddkver.h>
#include <d3d12.h>
#include <wrl.h>
#include <string>
#include <d3dcompiler.h>
#include <unordered_map>
#include "d3dx12.h"
#include "ComPtr.h"
#include "RenderData.h"

// Shader key type definition
using ShaderKey = std::tuple<std::wstring, std::string, std::string, uint64_t>;

// Shader key hash function
struct ShaderKeyHash
{
    std::size_t operator()(const ShaderKey& key) const
    {
        std::size_t h1 = std::hash<std::wstring>{}(std::get<0>(key));
        std::size_t h2 = std::hash<std::string>{}(std::get<1>(key));
        std::size_t h3 = std::hash<std::string>{}(std::get<2>(key));
        std::size_t h4 = std::hash<uint64_t>{}(std::get<3>(key));
        return h1 ^ (h2 << 1) ^ (h3 << 2) ^ (h4 << 3);
    }
};

// Shader description structure
struct ShaderDesc
{
    std::wstring filePath;
    std::string entryPoint;
    std::string profile;
    uint64_t allowedDefines;
};

static const ShaderDesc VS_TABLE[] = {
    { L"VertexShader.hlsl", "BasicVS", "vs_5_0" },      // VS_ID::Basic = 0
	{ L"VertexShader.hlsl", "PostEffectVS", "vs_5_0" }, // VS_ID::PostEffect = 1
};

static const ShaderDesc PS_TABLE[] = {
    { L"PixelShader.hlsl", "BasicPS", "ps_5_0" },           // PS_ID::Basic = 0
    { L"PixelShader.hlsl", "BBScreenEffectPS", "ps_5_0" },  // PS_ID::Basic = 0
	{ L"PixelShader.hlsl", "PostEffectPS", "ps_5_0" },      // PS
};

// ShaderLibrary class
class ShaderLibrary
{
public:
    ShaderLibrary();
    ~ShaderLibrary() { m_shaderCache.clear(); };

	// Get vertex/pixel shader
    Microsoft::WRL::ComPtr<ID3DBlob> GetVS(VS_ID vsId, uint64_t defines = 0);
    Microsoft::WRL::ComPtr<ID3DBlob> GetPS(PS_ID psId, uint64_t defines = 0);

private:
    std::unordered_map<ShaderKey, Microsoft::WRL::ComPtr<ID3DBlob>, ShaderKeyHash> m_shaderCache{};

private:
	// Get or compile shader
    Microsoft::WRL::ComPtr<ID3DBlob> GetOrCompileShader(
		const ShaderDesc& desc,
		uint64_t defines = 0
	);

    static std::vector<D3D_SHADER_MACRO> BuildMacros(uint64_t defines);
};