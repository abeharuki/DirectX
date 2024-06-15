#pragma once
#include "math.h"
#include <Windows.h>
#include <d3d12.h>
#include <string>
#include <wrl.h>
#include <dxcapi.h>
#include "StringUtility.h"
#include "WorldTransform.h"
#include "DirectXCommon.h"
#include "Mesh.h"
#include "GraphicsPipeline.h"
#include "TextureManeger.h"
#include "Engine.h"
#include "WorldTransform.h"
#include <imgui.h>
#include "GraphicsPipeline.h"
#include <ViewProjection.h>

class Skybox {
public: // サブクラス
	/// <summary>
	/// 頂点データ構造体
	/// </summary>
	struct VertexPosUv {
		Vector3 pos; // xyz座標
		Vector2 uv;  // uv座標
	};

	/// <summary>
	/// 定数バッファ用データ構造体
	/// </summary>
	struct ConstBufferData {
		Vector4 color; // 色 (RGBA)
		Matrix4x4 mat; // ３Ｄ変換行列
	};

public: // 静的メンバ関数

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Skybox* GetInstance();

	static Skybox* Create(const std::string& fileName);

	uint32_t GetTexture() { return texture_; }

public: // 静的メンバ変数
	// 頂点数
	static const int kVertNum = 4;

	static Skybox* instance_;

	static uint32_t textureNum;

	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;


	// 射影行列
	static Matrix4x4 sMatProjection_;

	uint32_t textureWidth = 0;
	uint32_t textureHeight = 0;


	BlendMode blendMode_ = BlendMode::kNormal;

public: // メンバ関数

	/// <summary>
	/// グラフィックスパイプラインの初期化
	/// </summary>
	void sPipeline();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <returns>成否</returns>
	void Initialize(const std::string& fileName);


	/// </summary>
	void Draw(WorldTransform& worldTransform, const ViewProjection& viewProjection);


private: // メンバ変数

	WorldTransform worldTransform_;

	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource;
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;

	VertexData* vertexData = nullptr;
	ModelData modelData;
	MaterialD* materialData = nullptr;
	CameraForGPU* cameraData = nullptr;

	uint32_t texture_;
private: // メンバ関数
	/// <summary>
	/// 頂点データ転送
	/// </summary>
	void CreateVertexResource();
	void LoadTexture(const std::string& fileName);
};