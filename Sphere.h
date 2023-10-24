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

class Sphere{
private: // 静的メンバ変数
	
	// ルートシグネチャ
	static Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	static Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	static Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	static Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

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
	void DrawSphere(VertexData* vertexData);

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>

	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);
	
	const D3D12_VIEWPORT& GetViewp() const { return viewport; }
	const D3D12_RECT& GetScissor() const { return scissorRect; }

	static Sphere* CreateSphere(const std::string& texturePath);

public:
	TextureManager* textureManager_;
	uint32_t texture_;

	

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	// ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	// WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce_;
	// マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	// データを書き込む
	TransformationMatrix* wvpData;
	VertexData* vertexData = nullptr;
	ModelData modelData;
	Material* materialData = nullptr;
	DirectionalLight* directionalLightData = nullptr;

	

private:
	// DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture(const std::string& texturePath);
};

