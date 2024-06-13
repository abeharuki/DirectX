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
#include "Engine.h"
#include "TextureManeger.h"
#include <DirectXTex.h>
#include <d3dx12.h>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "GraphicsPipeline.h"
#include "ModelManager.h"



struct OutLineStyle {
	
	int32_t isEnable;
	float differenceValue;	
	float padding1[2];
	Matrix4x4 projectionInverse;
	
};


class OutLine
{
public:
	static OutLine* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw();

	void Apply();

	//エフェクトの設定
	void isOutLine(bool flag) { outLineData->isEnable = flag; }
	void ValueOutLine(float value) { outLineData->differenceValue = value; }
	void SetViewProjection(ViewProjection& viewProjection) { 
		viewProjection_ = viewProjection;
		outLineData->projectionInverse = Math::Inverse(viewProjection_.matProjection);
	}
private:
	void CreateResource();

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

private:
	static OutLine* instance_;
	ViewProjection viewProjection_;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	// リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> postEffectResource_;

	OutLineStyle* outLineData = nullptr;


	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

};