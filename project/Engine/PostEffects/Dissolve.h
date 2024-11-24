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

struct DissolveStyle {
	Matrix4x4 uvTransform;
	float threshold;
	Vector3 edgeColor;
	int32_t isGradient;
	int32_t isEnble;
	float pading[2];
};

class Dissolve {
public:

	void Initialize();

	void Draw(DescriptorHandle srvHandle);

	void PreDraw();
	void PostDraw();


	const DescriptorHandle& GetHandle() { return colorBuffer_->GetSRVHandle(); }

	void isDissolve(bool flag) { dissolveData->isEnble = flag; }
	//閾値
	void Threshold(float num) { dissolveData->threshold = num; }
	void SetColor(Vector3 num) { dissolveData->edgeColor = num; }
private:
	void CreateResource();

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

	//マスクテクスチャの読み込み
	void LoadTexture(const std::string& texturePath);
private:

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

	DissolveStyle* dissolveData = nullptr;

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