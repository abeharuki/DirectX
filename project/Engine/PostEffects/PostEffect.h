#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
#include <fstream>
#include <sstream>
#include <dxcapi.h>
#include "Engine.h"
#include "TextureManeger.h"
#include <DirectXTex.h>
#include <d3dx12.h>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "GraphicsPipeline.h"
#include "ModelManager.h"

struct Grayscale {
	int32_t isEnable;
	float padding[3];
};

struct Vignetting {
	int32_t isEnable;
	Vector3 color;
	float intensity;//明るさ
	float padding[3];
};

struct Smoothing
{
	int32_t isEnable;
	float kernelSize;
};

struct PostEffects {
	Grayscale grayscal;
	Vignetting vignetting;
	Smoothing smoothing;
};


class PostEffect
{
public:
	static PostEffect* GetInstance();

	static void Destroy();

	void Initialize();

	void Draw();

	//エフェクトの設定
	void isGrayscale(bool flag) { postEffectData->grayscal.isEnable = flag; }
	void isVignetting(bool flag) { postEffectData->vignetting.isEnable = flag; }
	void VignetteIntensity(float intensity) { postEffectData->vignetting.intensity = intensity; }
	void VignetteColor(Vector3 color) { postEffectData->vignetting.color = color; }
	void Vignette(Vignetting vigne) { postEffectData->vignetting = vigne; }
	void isSmoothing(bool flag) { postEffectData->smoothing.isEnable = flag; }
	void SmoothingKernelSize(float size) { postEffectData->smoothing.kernelSize = size; }
private:
	PostEffect() = default;
	~PostEffect() = default;
	PostEffect(const PostEffect&) = delete;
	PostEffect& operator=(const PostEffect&) = delete;

	void CreateResource();

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

private:
	static PostEffect* instance_;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> postEffectResource_;

	PostEffects* postEffectData = nullptr;


	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

};