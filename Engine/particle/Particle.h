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
#include <random>

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

	BlendMode blendMode_ = BlendMode::kAdd;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Particle* GetInstance();

	// 初期化
	void Initialize(const std::string& filename, uint32_t Count);

	//スタート
	void Update();
	

	void sPipeline();


	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	void CreateVertexResource();

	void CreateInstanceSRV();

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);

	static Particle* Create(const std::string& filename,uint32_t Count);

	Particle_ MakeNewParticle(std::mt19937& randomEngine);
	
	//パーティクルループ
	void LoopParticles();

	//パーティクルの数
	void SetCount(int count) {
		instanceCount = count;
		particles[count];
	}

	// 色とアルファ値
	void SetColor(Vector4 color);

	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);

	//パーティクル速度
	void SetSpeed(float speed);

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
	ParticleForGPU* instancingData;
	ModelData modelData;
	Material* materialData = nullptr;

	TextureManager* textureManager_;
	uint32_t texture_;

    uint32_t instanceCount = 1;
	Particle_ particles[100];
	
	float kDeltaTime = 1.0f / 60.0f;
	
	bool loop_ = false;
	bool particle = false;

	uint32_t descriptorSizeSRV;

private:
	// DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& filename);

	
};