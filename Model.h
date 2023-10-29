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
	
	// ライティング
	static Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	static DirectionalLight* directionalLightData;

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

	
	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

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
	static void LightDraw(Vector4 color, Vector3 direction, float intensity);

	
	static void PostDraw();

	
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection, bool light);
	
	
	const D3D12_VIEWPORT& GetViewp() const { return viewport; }
	const D3D12_RECT& GetScissor() const { return scissorRect; }

	//static Model* CreateModelFromObj(const std::string& filename, const std::string& texturePath); 
	static Model* CreateModelFromObj(const std::string& filename, const std::string& texturePath);
	
	static Microsoft::WRL::ComPtr<ID3D12Resource> GetLightRsurce() { return lightResource_; };

	//色とアルファ値
	void SetColor(Vector4 color);

	//ブレンドモード
	void SetBlendMode(BlendMode blendMode);

private:
	
	
	
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	
	// WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce_;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	// データを書き込む
	TransformationMatrix* wvpData;
	VertexData* vertexData = nullptr;
	ModelData modelData;
	Material* materialData = nullptr;
	
	
	TextureManager* textureManager_;
	uint32_t texture_;

	uint16_t instanceCount = 10;
 private:
	//DirectX::ScratchImage LoadTexture(const std::string& filePath);
	 void LoadTexture(const std::string& filename, const std::string& texturePath);

	 // objファイルの読み込み
	 MaterialData LoadMaterialTemplateFile(const std::string& filename);
	 ModelData LoadObjFile(const std::string& filename);

	
};