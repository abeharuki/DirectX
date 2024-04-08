#pragma once
#include "WinApp.h"
#include "DirectXCommon.h"
#include "GraphicsPipeline.h"
#include <Math/math.h>
#include <Mesh.h>
#include <Engine.h>

class PostEffects
{
public:

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;


public:
	static PostEffects* GetInstance();

	static void Destroy();

	void Initialize();

	void Update();

	void Draw();


private:
	PostEffects() = default;
	~PostEffects() = default;
	PostEffects(const PostEffects&) = delete;
	PostEffects& operator=(const PostEffects&) = delete;

	void CreateVertexBuffer();

	void CreatePipelineState();

	BlendMode blendMode_ = BlendMode::kNone;
private:
	static PostEffects* instance_;

	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descHeapSRV;


	D3D12_VERTEX_BUFFER_VIEW vbView_;
	VertexData* vertexData_;
	bool isEnable_ = false;
};