#pragma once
#include <wrl.h>
#include "StringUtility.h"

#include <dxcapi.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include "DirectXCommon.h"

enum class BlendMode {
	kNone,
	kNormal,   //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	kAdd,      //!< 加算。Src * SrcA + Dest * 1
	kSubtract, //!< 減算。Dest * 1 - Src * SrcA
	kMultily,  //!< 乗算。Src * 0 + Dest * Src

	kCountOfBlendMode, //!< ブレンドモード数。指定はしない
};


class GraphicsPipeline {
public:
	//	シングルトンインスタンス
	static GraphicsPipeline* GetInstance();

public:
	

	
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
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(BlendMode blendMode_);
	// ParticleGraphicsPipelineの生成
	//Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateParticleGraphicsPipeline(BlendMode blendMode_);

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