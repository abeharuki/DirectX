#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
#include "Math/math.h"
#include <fstream>
#include <sstream>
#include <dxcapi.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include "GraphicsPipeline.h"
#include "ModelManager.h"
#include "base/ColorBuffer.h"

struct BloomStyle {
	float stepWidth;
	float sigma;
	float lightStrength;
	float bloomThreshold;
	int32_t isEnble;
};

class Bloom {
public:

	void Initialize();

	void Draw(DescriptorHandle srvHandle);

	void PreDraw();
	void PostDraw();


	const DescriptorHandle& GetHandle() { return colorBuffer_->GetSRVHandle(); }


	//テクスチャのサイズ
	void SetWidth(float num) { bloomData->stepWidth = num; }
	void SetSigma(float sigma) { bloomData->sigma = sigma; }
	void lightStrength(float strength) { bloomData->lightStrength = strength; }
	void Threshold(float threshold) { bloomData->bloomThreshold = threshold; }
	void isBloom(bool flag) { bloomData->isEnble = flag; }
private:
	void CreateResource();

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();
private:

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

	BloomStyle* bloomData = nullptr;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	std::unique_ptr<ColorBuffer> colorBuffer_ = nullptr;
	float clearColor_[4]{ 0.1f, 0.25f, 0.5f, 1.0f };

	uint32_t texture_;
};
