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
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"
#include "WorldTransform.h"
#include "ViewProjection.h"

class Model {
private: // 静的メンバ変数
	
	

	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;
	
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
	static Model* Create();
	void CreateVertexResource();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	static void PreDraw();

	
	static void PostDraw();

	
	void Draw(WorldTransform& worldTransform,const ViewProjection& viewProjection);
	static void Draw(WorldTransform& worldTransform,Model*model /* const ViewProjection& viewProjection*/);
	
	
	const D3D12_VIEWPORT& GetViewp() const { return viewport; }
	const D3D12_RECT& GetScissor() const { return scissorRect; }

	//static Model* CreateModelFromObj(const std::string& filename, const std::string& texturePath); 
	static Model* CreateModelFromObj(const std::string& filename, const std::string& texturePath);
	

private:
	TextureManager* textureManager_;
	uint32_t texture_;

	ModelData modelData_;
	std::unique_ptr<Mesh> mesh_;
	
	
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
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	
	VertexData* vertexData = nullptr;
	ModelData modelData;
	TransformationMatrix* wvpData;
	Material* materialData = nullptr;
	DirectionalLight* directionalLightData = nullptr;
	
	static Transform transform;

	static Transform cameraTransform;

	//Texture
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
 private:
	//DirectX::ScratchImage LoadTexture(const std::string& filePath);
	 void LoadTexture(const std::string& filename, const std::string& texturePath);
	 D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index);
};