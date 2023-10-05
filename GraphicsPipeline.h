#pragma once
#include <wrl.h>
#include "StringUtility.h"

#include <dxcapi.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

class GraphicsPipeline {
public:
	//	シングルトンインスタンス
	static GraphicsPipeline* GetInstance();

public:
	

	static ID3D12DescriptorHeap* SRVHeap;
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	static Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	static Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

public:
	//	GraphicsPipelineの生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline();

	//	RootSignatureの生成
	void CreateRootSignature(D3D12_ROOT_PARAMETER* rootParameter, UINT num);
	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateVSShader(const std::string& vsFileName);
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePSShader(const std::string& vsFileName);









};