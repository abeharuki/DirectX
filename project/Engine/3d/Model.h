#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
#include "Material.h"
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
#include <Skybox.h>
#include <PostEffects/Dissolve.h>

struct WritingStyle {
	DirectionLight directionLight_;
	PointLight pointLight_;
	SpotLight spotLight_;
	Environment  environment_;
	DissolveStyle dissolve_;
};

class Model {
public: // 静的メンバ変数
	
	

	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;
	
	// ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;
	
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	
	//static Microsoft::WRL::ComPtr<ID3D12Resource> publicLightResource_;
	//static WritingStyle* publicLightData;

	BlendMode blendMode_ = BlendMode::kNormal;

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Model* GetInstance();

	//初期化
	static void StaticInitialize();
	void Initialize(const std::string& filename, const std::string& texturePath);
	void Initialize(const std::string& filename);
	void NoDepthInitialize(const std::string& filename, const std::string& texturePath);

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();
	void sNoDepthPipeline();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	void CreateVertexResource();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	//光の色　向き　明るさ
	void DirectionalLightDraw(DirectionLight directionLight);
	//ポイントライトの詳細　向き
	void PointLightDraw(PointLight pointLight, Vector3 direction);
	//スポットライト
	void SpotLightDraw(SpotLight spotLight);

	void DirectionalLight(Vector4 color, Vector3 direction, float intensity);

	
	static void PostDraw();

	
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);
	
	

	
	static Model* CreateModelFromObj(const std::string& filename, const std::string& texturePath);
	static Model* CreateFromObj(const std::string& filename);
	static Model* CreateFromNoDepthObj(const std::string& filename, const std::string& texturePath);

	//static Microsoft::WRL::ComPtr<ID3D12Resource> GetLightRsurce() { return lightResource_; };

	void SetUV(Transform& uvTransform) { materialData_->SetUV(uvTransform); }
	void SetMaskUV(Transform& uvTransform) { 
		// UVTransform用の行列
		Matrix4x4 uvTransformMatrix = Math::MakeAffineMatrix(
			{
				uvTransform.scale.x + 1,
				uvTransform.scale.y + 1,
				uvTransform.scale.z + 1,
			},
			uvTransform.rotate, uvTransform.translate);
		lightData->dissolve_.uvTransform = uvTransformMatrix;
	}

	//色とアルファ値
	void SetColor(Vector4 color);
	

	//ブレンドモード
	void SetBlendMode(BlendMode blendMode);

	//光沢度
	void SetShininess(float i);

	//映り込み度
	void Environment(float environment, bool flag) {
		if (Skybox::textureNum != 0) {
			lightData->environment_.isEnble_ = flag;
		}

		lightData->environment_.environment = environment;
	}

	void IsGradient(bool flag) { lightData->dissolve_.isGradient = flag; }
	void SetThreshold(float num) { lightData->dissolve_.threshold = num; }
	void SetEdgeColor(Vector3 color) { lightData->dissolve_.edgeColor = color; }
	void SetMaskTexture(const std::string& texturePath);
	void SetTexture(const std::string& texturePath);

private:
	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResorce_;

	// ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	WritingStyle* lightData;

	//ディゾルブ
	Microsoft::WRL::ComPtr<ID3D12Resource> dissolveResource_;
	DissolveStyle* dissolveData = nullptr;

	std::unique_ptr<Mesh> meshData_;
	//マテリアルデータ
	std::unique_ptr<Material> materialData_;
	ModelData modelData;
	CameraForGPU* cameraData = nullptr;
	
	TextureManager* textureManager_;
	uint32_t texture_;
	uint32_t maskTexture_;
	uint32_t skyboxTexture_;
	uint16_t instanceCount = 10;
 private:
	
	void LoadTexture(const std::string& texturePath);

	 // objファイルの読み込み
	
};