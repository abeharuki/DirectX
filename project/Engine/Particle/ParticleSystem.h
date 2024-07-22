#pragma once
#pragma once
#include "Engine.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "StringUtility.h"
#include "TextureManeger.h"
#include "ViewProjection.h"
#include <DirectXTex.h>
#include <d3d12.h>
#include <d3dx12.h>
#include <dxcapi.h>
#include <fstream>
#include <memory>
#include <sstream>
#include <wrl.h>
#include <random>
#include "ModelManager.h"
#include <Material.h>

class ParticleSystem {
public: // 静的メンバ変数
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CSRootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CSPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	BlendMode blendMode_ = BlendMode::kAdd;
	
public:
	
	// 初期化
	void Initialize(const std::string& filename, Emitter emitter);

	// スタート
	void Update();

	void Draw(const ViewProjection& viewProjection);

	static ParticleSystem*Create(const std::string& filename, Emitter emitter);

	// パーティクルループ
	void StopParticles();

	void SetTranslate(Vector3 transform) { emitter_.transform.translate = transform; }

	// 色とアルファ値
	void SetColor(Vector4 color);

	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);

	// パーティクル速度
	void SetSpeed(float speed);

	//フィールドをセット
	void SetFiled(AccelerationField accelerationField);

	void SetTexture(const std::string& filename);
	void SetModel(const std::string& filename, std::string& path);
private:

	void sPipeline();

	void CreateResource();

	void CreateVertexResource();

	void CreateInstanceResource();

	void UpdatePerViewResource(const ViewProjection& viewProjection);

	void LoadTexture(const std::string& filename);

	Particle MakeNewParticle(std::mt19937& randomEngine, const Transform transform);

	std::list<Particle> Emission(const Emitter& emitter, std::mt19937& randomEngine);

private:
	//インスタンスの最大数
	const uint32_t kNumMaxInstance = 100;

	std::unique_ptr<Material> materialData_;
	std::unique_ptr<Mesh> meshData_;
	std::unique_ptr<RWStructuredBuffer> particleResource_ = nullptr;
	std::unique_ptr<StructuredBuffer> instancingResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_ = nullptr;


	// データを書き込む
	ParticleForGPU* instancingData_;
	PerView* perViewData_;
	ModelData modelData;
	

	uint32_t texture_;
	
	std::list<Particle> particles;

	Emitter emitter_{};
	AccelerationField accelerationField_;

	float kDeltaTime = 1.0f / 60.0f;
	
	bool loop_ = false;
	bool particle = false;

	Vector4 color_;
	bool isColor = false;

	std::random_device seedGenerator;

};