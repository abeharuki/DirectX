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
	Microsoft::WRL::ComPtr<ID3D12RootSignature> skyboxRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> lineRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> spriteRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> particleRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> animationRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> animationCSRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> postEffectRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> outLineRootSignature_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> dissolveRootSignature_ = nullptr;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> skyboxPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> linePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> spritePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> particlesPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> animationPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> animationCSPipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> outLinePipelineState_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> dissolvePipelineState_ = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_ = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_ = nullptr;
	//3dObj
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_ = nullptr;
	//skybox
	Microsoft::WRL::ComPtr<IDxcBlob> skyboxVertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> skyboxPixelShaderBlob_ = nullptr;
	//Line
	Microsoft::WRL::ComPtr<IDxcBlob> linePixelShaderBlob_ = nullptr;
	//Sprite
	Microsoft::WRL::ComPtr<IDxcBlob> vertexSpriteShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelSpriteShaderBlob_ = nullptr;
	//Particle
	Microsoft::WRL::ComPtr<IDxcBlob> particleVertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> particlePixelShaderBlob_ = nullptr;
	//animation
	Microsoft::WRL::ComPtr<IDxcBlob> animationVertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> animationComputeShaderBlob_ = nullptr;
	//PostEffect
	Microsoft::WRL::ComPtr<IDxcBlob> postEffectVertexShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> outLinePixelShaderBlob_ = nullptr;
	Microsoft::WRL::ComPtr<IDxcBlob> dissolvePixelShaderBlob_ = nullptr;
public:
	//	GraphicsPipelineの生成
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateSkyboxGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateLineGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateSpritePipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateParticleGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateAnimationGraphicsPipeline(BlendMode blendMode_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateAnimationCSGraphicsPipeline();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreatePostEffectGraphicsPipeline(Microsoft::WRL::ComPtr<IDxcBlob> postEffectPixelShaderBlob_);
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateOutLineGraphicsPipeline();
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CreateDissolveGraphicsPipeline();

	//	RootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateSpriteRootSignature();
	//SkyCube
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateSkyboxRootSignature();
	//	LineRootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateLineRootSignature();
	// ParticleRootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateParticleRootSignature();
	// AnimationRootSignatureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateAnimationRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreateAnimationCSRootSignature();
	//PostEffect系RootSignetureの生成
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CreatePostEffectRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateOutLineRootSignature();
	Microsoft::WRL::ComPtr<ID3D12RootSignature>CreateDissolveRootSignature();

	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePSShader();
	//	SkyCubvertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateSkyboxVSShader();
	//	SkyCubepixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateSkyboxPSShader();

	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateLinePSShader();

	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateSpriteVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateSpritePSShader();

	//	ParticleVertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateParticleVSShader();
	//	ParticlePixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateParticlePSShader();

	//	animationVertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateAnimationVSShader();
	//ComputeShaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreateAnimationCSShader();

	//	vertexshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePostEffectVSShader();
	//	pixelshaderの生成
	Microsoft::WRL::ComPtr<IDxcBlob> CreatePostEffectPSShader(const std::wstring path);
	Microsoft::WRL::ComPtr<IDxcBlob> CreateOutLinePSShader();
	Microsoft::WRL::ComPtr<IDxcBlob> CreateDissolvePSShader();

	static IDxcBlob* CompileShader(
		// CompilerするShaderファイルへのパス
		const std::wstring& filePath,
		// Compilerに使用するProfile
		const wchar_t* profile,
		// 初期化で生成したものを3つ
		IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);




};