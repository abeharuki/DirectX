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
#include <WorldTransform.h>
#include <LineBox.h>

struct Range
{
	Vector3 min;
	float padding1;
	Vector3 max;
	float padding2;
};

struct EmitterSphere {
	Vector3 translate;//位置
	float radius;//射出半径
	uint32_t count;//射出数
	float frequency;//射出間隔
	float frequencyTime;//射出間隔調整時間
	uint32_t emit;//射出許可
	Range scaleRange;
	Range translateRange;
	Range colorRange;
	Range velocityRange;
};

class ParticleSystem {
public: // 静的メンバ変数
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> CSRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> emiteCSRootSignature_;
	Microsoft::WRL::ComPtr<ID3D12RootSignature> updateParticleCSRootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> CSPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> emiteCSPipelineState_;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> updateParticleCSPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

	BlendMode blendMode_ = BlendMode::kAdd;
	
	//パーティクルの最大数
	static const uint32_t kMaxParticles = 1024;

public:
	
	static ParticleSystem* GetInstance();
	static void Destroy();
	// 初期化
	void Initialize(const std::string& filename);

	// スタート
	void Update();

	void Draw(const ViewProjection& viewProjection);

	static ParticleSystem*Create(const std::string& filename);

	// パーティクルループ
	void StopParticles();

	void SetEmitter(EmitterSphere emite) { 
		emitterSphere_->translate = emite.translate;
		emitterSphere_->count = emite.count;
		emitterSphere_->frequency = emite.frequency;
		emitterSphere_->scaleRange = emite.scaleRange;
		emitterSphere_->translateRange = emite.translateRange;
		emitterSphere_->colorRange = emite.colorRange;
		emitterSphere_->velocityRange = emite.velocityRange;
	}

	void SetTranslate(Vector3 transform) { emitterSphere_->translate = transform; }

	// 色とアルファ値
	void SetColor(Range color) { emitterSphere_->colorRange = color; };

	// パーティクル速度
	void SetSpeed(Range speed) { emitterSphere_->velocityRange = speed; };

	//フィールドをセット
	void SetFiled(AccelerationField accelerationField);

	void SetTexture(const std::string& filename);
	void SetModel(const std::string& filename, std::string& path);
private:

	void InitilaizeCS();
	void UpdateCS();


	void sPipeline();

	void CreateResource();

	void CreateVertexResource();

	void UpdatePerViewResource(const ViewProjection& viewProjection);

	void LoadTexture(const std::string& filename);

private:
	static ParticleSystem* instance_;

	//インスタンスの最大数
	const uint32_t kNumMaxInstance = 100;

	std::unique_ptr<Material> materialData_;
	std::unique_ptr<Mesh> meshData_;
	std::unique_ptr<RWStructuredBuffer> particleResource_ = nullptr;
	std::unique_ptr<RWStructuredBuffer> freeListIndexResource_ = nullptr;
	std::unique_ptr<RWStructuredBuffer> freeListResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> perViewResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> emitterResource_ = nullptr;
	Microsoft::WRL::ComPtr<ID3D12Resource> perFrameResource_ = nullptr;

	// データを書き込む
	//ParticleForGPU* instancingData_;
	PerView* perViewData_;
	ModelData modelData;
	EmitterSphere* emitterSphere_;
	PerFrame* perFrame_;
	

	uint32_t texture_;
	
	std::list<Particle> particles;
	AccelerationField accelerationField_;

	float kDeltaTime = 1.0f / 60.0f;
	
	bool loop_ = false;
	bool particle = false;

	bool initializeCS_ = false;

	WorldTransform worldTransform_;
	std::unique_ptr<LineBox> linebox_;
};