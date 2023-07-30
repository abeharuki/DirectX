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
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> sCommandList_;
	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	
	Microsoft::WRL::ComPtr<ID3DBlob> signatureBlob_;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	
	

public:
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Model* GetInstance();

	//初期化
	void Initializ(DirectXCommon* dxCommon);

	/// <summary>
	/// 静的初期化
	/// </summary>
    void StaticInitialize();

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void InitializeGraphicsPipeline();

	/// <summary>
	/// 3Dモデル生成
	/// </summary>
	/// <returns></returns>
	static Model* Create();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="commandList">描画コマンドリスト</param>
	void PreDraw();

	
	IDxcBlob* CompileShader(
	    // CompilerするShaderファイルへのパス
	    const std::wstring& filePath,
	    // Compilerに使用するProfile
	    const wchar_t* profile,
	    // 初期化で生成したものを3つ
	    IDxcUtils* dxcUtils, IDxcCompiler3* dxcCompiler, IDxcIncludeHandler* includeHandler);

	const D3D12_VIEWPORT& GetViewp() const { return viewport; }
	const D3D12_RECT& GetScissor() const { return scissorRect; }


private:
	DirectXCommon* dxCommon_;
	Utility* utility_;
	Mesh* mesh_;
	Math* math_;

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	D3D12_VERTEX_BUFFER_VIEW vbView2_ = {};
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	// ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	// WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce_;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	//メタボール
	Microsoft::WRL::ComPtr<ID3D12Resource> metaball_;
	Microsoft::WRL::ComPtr<ID3D12Resource> transformMetaBall_;
	VertexData* vertexData = nullptr;
	TransformationMatrix* wvpData = nullptr;
	DirectionalLight* directionalLightData = nullptr;
	Material* materialData = nullptr;
	VertexData* metaballData = nullptr;
	TransformationMatrix* transformationmetaBallData = nullptr;
	Transform transform{
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };

	Transform cameraTransform{
	    {1.0f, 1.0f, 1.0f  },
        {0.0f, 0.0f, 0.0f  },
        {0.0f, 0.0f, -10.0f}
    };

	Transform metaBalltransform{
	    {1.0f, 1.0f, 1.0f},
        {0.0f, 0.0f, 0.0f},
        {0.0f, 0.0f, 0.0f}
    };


	//Texture
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU;
 private:
	DirectX::ScratchImage LoadTexture(const std::string& filePath);
	void LoadTexture();
};