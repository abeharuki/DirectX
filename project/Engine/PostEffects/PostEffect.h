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
#include "OutLine.h"

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

struct Gaussian
{
	int32_t isEnable;
	float kernelSize;
	float padding[2];
};

struct RadialBlur {
	Vector2 center;
	float blurWidth;
	int32_t isEnble;
};



struct PostEffects {
	Grayscale grayscal;
	Vignetting vignetting;
	Gaussian gaussian;
	RadialBlur radialBlur;
};


class PostEffect
{
public:
	static PostEffect* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw();

	void Apply();

	//エフェクトの設定
	void Effect(bool flag) { effect_ = flag; }
	//グレイスケール
	void isGrayscale(bool flag) { postEffectData->grayscal.isEnable = flag; }
	//ビネット
	void isVignetting(bool flag) { postEffectData->vignetting.isEnable = flag; }
	void VignetteIntensity(float intensity) { postEffectData->vignetting.intensity = intensity; }
	void VignetteColor(Vector3 color) { postEffectData->vignetting.color = color; }
	void Vignette(Vignetting vigne) { postEffectData->vignetting = vigne; }
	//ガウシアン
	void isGaussian(bool flag) { postEffectData->gaussian.isEnable = flag; }
	void GaussianKernelSize(float size) { postEffectData->gaussian.kernelSize = size; }
	//アウトライン
	void isOutLine(bool flag,ViewProjection& viewProjection) { outline_->isOutLine(flag); outline_->SetViewProjection(viewProjection); }
	void ValueOutLine(float value) { outline_->ValueOutLine(value); }
	//ラジアル
	void isRadialBlur(bool flag) { postEffectData->radialBlur.isEnble = flag; }
	void RadialBlurCenter(Vector2 num) { postEffectData->radialBlur.center = { num.x,num.y };}
	void RadialBlurWidth(float num) { postEffectData->radialBlur.blurWidth = num; }

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
	std::unique_ptr<OutLine> outline_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	bool effect_ = true;
};