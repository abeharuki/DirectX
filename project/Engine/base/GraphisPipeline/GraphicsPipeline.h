#pragma once
#include <wrl.h>
#include "StringUtility.h"

#include <dxcapi.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include "DirectXCommon.h"

enum class BlendMode {
	kNone,
	kNormal,   //!< 通常αブレンド。デフォルト。 Src * SrcA + Dest * (1 - SrcA)
	kAdd,      //!< 加算。Src * SrcA + Dest * 1
	kSubtract, //!< 減算。Dest * 1 - Src * SrcA
	kMultily,  //!< 乗算。Src * 0 + Dest * Src

	kCountOfBlendMode, //!< ブレンドモード数。指定はしない
};

enum class Pipeline {
	kNormal,   
	kParticle,      
	
};

class GraphicsPipeline {
public:
	//	シングルトンインスタンス
	static GraphicsPipeline* GetInstance();

public:
	

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> particleRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> postEffectRootSignature_ = nullptr;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> spritePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> particlesPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> postEffectPipelineState_ = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_ = nullptr;
	//3dObj
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	//Sprite
	Microsoft::WRL::ComPtr<IDxcBlob> vertexSpriteShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelSpriteShaderBlob_ = nullptr;
	//Particle
	Microsoft::WRL::ComPtr<IDxcBlob> particleVertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> particlePixelShaderBlob_ = nullptr;
	//PostEffect
	Microsoft::WRL::ComPtr<IDxcBlob> postEffectVertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> postEffectPixelShaderBlob_ = nullptr;

public:
	//	GraphicsPipelineの生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateSpritePipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateParticleGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePostEffectGraphicsPipeline();
	
	//	RootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateRootSignature();
	// ParticleRootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateParticleRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreatePostEffectRootSignature();

	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePSShader();

	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateSpriteVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateSpritePSShader();

	//	ParticleVertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateParticleVSShader();
	//	ParticlePixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateParticlePSShader();

	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePostEffectVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePostEffectPSShader();

	static IDxcBlob* CompileShader(
	    // CompilerするShaderファイルへのパス
	    const std::wstring& filePath,
	    // Compilerに使用するProfile
	    const wchar_t* profile,
	    // 初期化で生成したものを3つ
	    IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);




};