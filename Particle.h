#pragma once
#pragma once
#include "Engine.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "StringUtility.h"
#include "TextureManeger.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <DirectXTex.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxcapi.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <wrl.h>

class Particle {
public: // 静的メンバ変数
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	BlendMode blendMode_ = BlendMode::kNone;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Particle* GetInstance();

	// 初期化
	
	void Initialize(const std::string& filename, uint32_t Count);

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	void CreateVertexResource();

	void CreateInstanceSRV();

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);

	

	// static Model* CreateModelFromObj(const std::string& filename, const std::string&
	// texturePath);
	static Particle* Create(const std::string& filename,uint32_t Count);

	
	// 色とアルファ値
	void SetColor(Vector4 color);

	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);

private:
	
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	// WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResouce_;
	// マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;

	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	uint32_t* indexData_;

	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandelCPU;
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandelGPU;

	
	// データを書き込む
	TransformationMatrix* instancingData;
	ModelData modelData;
	Material* materialData = nullptr;

	TextureManager* textureManager_;
	uint32_t texture_;

    uint32_t instanceCount = 1;
	Transform transforms[1];


	uint32_t descriptorSizeSRV;

private:
	// DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& filename);

	
};