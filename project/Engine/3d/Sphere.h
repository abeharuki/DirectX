#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
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

class Sphere{
 // 静的メンバ変数
public:
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	static Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	static Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	BlendMode blendMode_ = BlendMode::kNormal;

	// ライティング
	static Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	static WritingStyle* lightData;

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
	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);
	
	//光の色　向き　明るさ
	static void LightDraw(Vector4 color, Vector3 direction, float intensity);


	static Sphere* CreateSphere(const std::string& texturePath);

	void SetColor(Vector4 color);
	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);
	// 光沢度
	void SetShininess(float i);

private:
	TextureManager* textureManager_;
	uint32_t texture_;


	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// カメラ用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> cameraResorce_;
	// マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	// データを書き込む
	TransformationMatrix* wvpData;
	VertexData* vertexData = nullptr;
	ModelData modelData;
	Material* materialData = nullptr;
	CameraForGPU* cameraData = nullptr;

	const uint32_t kSubdivision = 128;
	uint32_t vertexIndex = (kSubdivision * kSubdivision) * 6;

private:
	// DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& texturePath);
};

