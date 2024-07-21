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
	void Initialize(const std::string& filename, Emitter emitter);

	// スタート
	void Update();

	void Draw(const ViewProjection& viewProjection);

	static Particle*Create(const std::string& filename, Emitter emitter);

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

	void UpdateBillboard(const ViewProjection& viewProjection);

	void sPipeline();

	void CreateVertexResource();

	void CreateInstanceSRV();

	void LoadTexture(const std::string& filename);

	Particle_ MakeNewParticle(std::mt19937& randomEngine, const Transform transform);

	std::list<Particle_> Emission(const Emitter& emitter, std::mt19937& randomEngine);

private:


	// WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> instancingResouce_;
	// マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;

	// データを書き込む
	ParticleForGPU* instancingData;
	ModelData modelData;
	MaterialD* materialData = nullptr;
	std::unique_ptr<Mesh> meshData_;

	uint32_t texture_;
	uint32_t instancing_;

	uint32_t instanceCount = 1;
	std::list<Particle_> particles;

	Emitter emitter_{};
	AccelerationField accelerationField_;

	float kDeltaTime = 1.0f / 60.0f;
	
	bool loop_ = false;
	bool particle = false;

	Vector4 color_;
	bool isColor = false;

	std::random_device seedGenerator;
	uint32_t descriptorSizeSRV;


};