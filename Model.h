#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"

#include <dxcapi.h>
#include "DirectXCommon.h"
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

class Model {
private: // 静的メンバ変数
	
	

	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;
	// コマンドリスト
	static ID3D12GraphicsCommandList* sCommandList_;
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
	void Initialize(DirectXCommon* dxCommon);

	/// <summary>
	/// 静的初期化
	/// </summary>
	static void StaticInitialize();

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
	static void PreDraw(ID3D12GraphicsCommandList* commandList);

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="worldTransform">ワールドトランスフォーム</param>
	/// <param name="viewProjection">ビュープロジェクション</param>
	void Draw(/* const WorldTransform& worldTransform, const ViewProjection& viewProjection*/);

	
	
	const D3D12_VIEWPORT& GetViewp() const { return viewport; }
	const D3D12_RECT& GetScissor() const { return scissorRect; }


private:
	DirectXCommon* dxCommon_;
	Utility* utility_;
	Mesh* mesh_;
	

	static D3D12_VIEWPORT viewport;
	static D3D12_RECT scissorRect;
	// 頂点バッファビュー
	static D3D12_VERTEX_BUFFER_VIEW vbView_;
	static D3D12_INDEX_BUFFER_VIEW ibView_;

	// 頂点
	static Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// ライティング
	static Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	// WVP用リソース
	static Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce_;
	//マテリアル用リソース
	static Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	
	VertexData* vertexData = nullptr;
	static TransformationMatrix* wvpData;
	Material* materialData = nullptr;
	DirectionalLight* directionalLightData = nullptr;
	
	static Transform transform;

	static Transform cameraTransform;
	
	


	//Texture
	static D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
 private:
	//DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture();
};