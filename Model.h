#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"

#include <dxcapi.h>
#include "DirectXCommon.h"

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
	void PreDraw(ID3D12GraphicsCommandList* commandList);

	
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
	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissorRect{};
};