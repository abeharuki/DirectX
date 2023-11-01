#pragma once
#include <wrl.h>
#include "StringUtility.h"

#include <dxcapi.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include "DirectXCommon.h"



class GraphicsPipeline {
public:
	//	シングルトンインスタンス
	static GraphicsPipeline* GetInstance();

public:
	

	static ID3D12DescriptorHeap* SRVHeap;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_ = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;

public:
	//	GraphicsPipelineの生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline();

	//	RootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateRootSignature();
	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePSShader();


	static IDxcBlob* CompileShader(
	    // CompilerするShaderファイルへのパス
	    const std::wstring& filePath,
	    // Compilerに使用するProfile
	    const wchar_t* profile,
	    // 初期化で生成したものを3つ
	    IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);




};