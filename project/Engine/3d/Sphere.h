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
#include "PostEffects/Dissolve.h"

/**
 * @file Sphere
 * @brief 球体モデルを管理するクラス
 */
class Sphere {
	// 静的メンバ変数
public:
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	static Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	static Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	BlendMode blendMode_ = BlendMode::kNormal;

	struct WritingStyle {
		DirectionLight directionLight_;
		PointLight pointLight_;
		SpotLight spotLight_;
		Environment  environment_;
		DissolveStyle dissolve_;
	};

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Sphere* GetInstance();

	// 初期化
	void Initialize(const std::string& texturePath);

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	void CreateVertexResource();
	void DrawSphere(VertexData* vertexData, uint32_t vertexIndex);
	
	//描画
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);

	//光の色　向き　明るさ
	void LightDraw(Vector4 color, Vector3 direction, float intensity);

	//光の色　向き　明るさ
	void DirectionalLightDraw(DirectionLight directionLight);
	//ポイントライトの詳細　向き
	void PointLightDraw(PointLight pointLight, Vector3 direction);
	//スポットライト
	void SpotLightDraw(SpotLight spotLight);

	static Sphere* CreateSphere(const std::string& texturePath);

	void SetColor(Vector4 color);
	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);
	// 光沢度
	void SetShininess(float i);

	//void isDissolve(bool flag) { lightData->dissolve_.isEnble = flag; }
	void SetThreshold(float num) { lightData->dissolve_.threshold = num; }
	void SetEdgeColor(Vector3 color) { lightData->dissolve_.edgeColor = color; }
	void SetMaskTexture(const std::string& texturePath);
	//ディゾルブの許可
	void IsGradient(bool flag) { lightData->dissolve_.isGradient = flag; }
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
private:
	TextureManager* textureManager_;
	uint32_t texture_;
	uint32_t maskTexture_;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResorce_;
	// データを書き込む
	TransformationMatrix* wvpData;
	VertexData* vertexData = nullptr;
	ModelData modelData;
	//マテリアルデータ
	std::unique_ptr<Material> materialData_;
	CameraForGPU* cameraData = nullptr;
	// ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	WritingStyle* lightData;
	const uint32_t kSubdivision_ = 128;
	uint32_t vertexIndex = (kSubdivision_ * kSubdivision_) * 6;

private:
	// DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& texturePath);
};

