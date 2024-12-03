#pragma once
#include "Engine.h"
#include "GraphicsPipeline.h"
#include "Mesh.h"
#include "StringUtility.h"
#include "TextureManeger.h"
#include "ViewProjection.h"
#include <DirectXTex.h>
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

struct Range1d
{
	float min;
	float max;
	float padding[2];
};

struct EmitterSphere {
	Vector3 translate;//位置
	float radius;//射出半径
	uint32_t count;//射出数
	float frequency;//射出間隔
	float frequencyTime;//射出間隔調整時間
	uint32_t emit;//射出許可
	float endAlpha;//アルファ値
	Vector3 endScale;//最終的なサイズ
	uint32_t isScaleChanging;//サイズを変えるか
	float padding[3];
	Range scaleRange;
	Range translateRange;
	Range colorRange;
	Range1d alphaRange;
	Range1d lifeTimeRange;
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

	BlendMode blendMode_ = BlendMode::kAdd; //BlendMode::kAdd;
	
	//パーティクルの最大数
	static const uint32_t kMaxParticles = 1024;
	//加速フィールドの最大数
	static const uint32_t kMaxAccelerationFields = 10;
	//重力フィールドの最大数
	static const uint32_t kMaxGravityFields = 10;

public:
	
	//static ParticleSystem* GetInstance();
	//static void Destroy();

	void Clear();

	// 初期化
	void Initialize(const std::string& filename);

	// スタート
	void Update();

	void Draw(const ViewProjection& viewProjection);

	//static ParticleSystem*Create(const std::string& filename);

	void StopParticle() { 
		emitterSphere_->frequencyTime = emitterSphere_->frequency; 
		emitterSphere_->emit = 0;
	}

	bool GetEmit() {
		if (emitterSphere_->emit == 1) {
			return true;
		}
		else {
			return false;
		}
		
	}


	//emitter
	void SetEmitter(EmitterSphere emite) { 
		emitterSphere_->translate = emite.translate;
		emitterSphere_->count = emite.count;
		emitterSphere_->frequency = emite.frequency;
		emitterSphere_->scaleRange = emite.scaleRange;
		emitterSphere_->translateRange = emite.translateRange;
		emitterSphere_->colorRange = emite.colorRange;
		emitterSphere_->alphaRange = emite.alphaRange;
		emitterSphere_->lifeTimeRange = emite.lifeTimeRange;
		emitterSphere_->velocityRange = emite.velocityRange;
		emitterSphere_->endAlpha = emite.endAlpha;
		emitterSphere_->endScale = emite.endScale;
		emitterSphere_->isScaleChanging = emite.isScaleChanging;
	}
	//座標
	void SetTranslate(Vector3 transform) { emitterSphere_->translate = transform; }
	//射出間隔
	void SetFrequency(float frequency) { emitterSphere_->frequency = frequency; }
	void SetFrequencyTime(float frequencyTime) { emitterSphere_->frequencyTime = frequencyTime; }
	// 色とアルファ値
	void SetColor(Range color) { emitterSphere_->colorRange = color; };
	void SetColor(Vector4 color){ materialData_->SetColor(color); }
	// パーティクル速度
	void SetSpeed(Range speed) { emitterSphere_->velocityRange = speed; };
	//フィールドをセット
	void SetAccelerationFiled(AccelerationField accelerationField);
	//フィールドをセット
	void SetGravityFiled(GravityField gravityField);
	//テクスチャ
	void SetTexture(const std::string& filename);
	//モデル
	void SetModel(const std::string& filename,const std::string& path);
	//ブレンドモード
	void SetBlendMode(BlendMode blendMode);

	void DebugParameter();
private:

	void InitilaizeCS();
	void UpdateCS();

	void sPipeline();

	void CreateResource();

	void CreateVertexResource();

	void UpdatePerViewResource(const ViewProjection& viewProjection);

	void LoadTexture(const std::string& filename);

private:
	//static ParticleSystem* instance_;

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

	std::unique_ptr<StructuredBuffer> accelerationFieldResource_ = nullptr;
	std::unique_ptr<StructuredBuffer> gravityFieldResource_ = nullptr;


	// データを書き込む
	PerView* perViewData_;
	ModelData modelData;
	EmitterSphere* emitterSphere_;
	PerFrame* perFrame_;
	AccelerationField* accelerationFieldData_;
	GravityField* gravityFieldData_;

	uint32_t texture_;
	
	std::list<Particle> particles;
	

	float kDeltaTime = 1.0f / 60.0f;
	
	bool loop_ = false;
	bool particle = false;

	bool initializeCS_ = false;

	WorldTransform worldTransform_;
	std::unique_ptr<LineBox> linebox_;

	//デバック用
	EmitterSphere emitter_;
	int particleCount_;
};