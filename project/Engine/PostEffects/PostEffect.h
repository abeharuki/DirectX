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
#include "Dissolve.h"
#include "GasianBlur.h"
#include "Bloom.h"

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

struct Random {
	float time;
	int32_t isEnble;
	float padding[2];
};

struct HSVMaterial
{
	float hue;
	float saturation;
	float value;
};

struct PostEffects {
	Grayscale grayscal;
	Vignetting vignetting;
	Gaussian gaussian;
	RadialBlur radialBlur;
	Random random;
	HSVMaterial material;
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
	void isOutLine(bool flag) { outline_->isOutLine(flag); /*outline_->SetViewProjection(viewProjection);*/ }
	void ValueOutLine(float value) { outline_->ValueOutLine(value); }
	//ラジアル
	void isRadialBlur(bool flag) { postEffectData->radialBlur.isEnble = flag; }
	void RadialBlurCenter(Vector2 num) { postEffectData->radialBlur.center = { num.x,num.y };}
	void RadialBlurWidth(float num) { postEffectData->radialBlur.blurWidth = num; }
	//ディゾルブ
	void isDissolve(bool flag) { dissolve_->isDissolve(flag); }
	void dissolveThreshold(float num) { dissolve_->Threshold(num); }
	void EdgeColor(Vector3 num) { dissolve_->SetColor(num); }
	//ノイズエフェクト
	void isRandom(bool flag) { postEffectData->random.isEnble = flag; }
	//ガウシアンブラー
	void SetBlurWidth(float num) { gasianBlur_->SetWidth(num); }
	void SetBlurSigma(float sigma) { gasianBlur_->SetSigma(sigma); }
	void isGasianBlur(bool flag) { gasianBlur_->isGasianBlur(flag); }
	//ブルーム
	void SetBloomWidth(float num) { bloom_->SetWidth(num); }
	void SetBloomSigma(float num) { bloom_->SetSigma(num); }
	void BloomLightStrength(float num) { bloom_->lightStrength(num); }
	void BloomTreshold(float num) { bloom_->Threshold(num); }
	void isBloom(bool flag) { bloom_->isBloom(flag); }
	//HSVフィルター
	void SetHsv(HSVMaterial hsv) { postEffectData->material = hsv; }
	void SetHsv(Vector3 hsv) { postEffectData->material.hue = hsv.x;  postEffectData->material.saturation = hsv.y;  postEffectData->material.value = hsv.y; }
	void SetHsvHue(float num){ postEffectData->material.hue = num; }
	void SetHsvSaturation(float num) { postEffectData->material.saturation = num; }
	void SetHsvValue(float num) { postEffectData->material.value = num; }
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
	std::unique_ptr<Dissolve> dissolve_;
	std::unique_ptr<GasianBlur> gasianBlur_;
	std::unique_ptr<Bloom> bloom_;


	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	bool effect_ = true;
};