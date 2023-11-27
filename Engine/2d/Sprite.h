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

class Sprite {
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
	/// 静的初期化
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="window_width">画面幅</param>
	/// <param name="window_height">画面高さ</param>
	void StaticInitialize(
	    ID3D12Device* device, int window_width, int window_height);

	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static Sprite* GetInstance();

	/// <summary>
	/// 描画前処理
	/// </summary>
	/// <param name="cmdList">描画コマンドリスト</param>
	static void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	static void PostDraw();

	
	
	void SetColor(Vector4 color);

	// ブレンドモード
	void SetBlendMode(BlendMode blendMode);

public: // 静的メンバ変数
	// 頂点数
	static const int kVertNum = 4;
	
	// デスクリプタサイズ
	static UINT sDescriptorHandleIncrementSize_;

	// ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;
	// パイプラインステートオブジェクト
	Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

	static Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
	static Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;
	
	
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


	/// <summary>
	/// 座標の設定
	/// </summary>
	/// <param name="position">座標</param>
	void SetPosition(const Vector2& position) { position_ = position; }
	void SetSize(const Vector2& size) { size_ = size_; }
	void SetRotation(const float& rotation) { rotation_ = rotation; }

	const Vector2& GetPosition() const { return position_; }

	
	/// <summary>
	/// 描画
	/// </summary>
	void Draw(Transform& transform,Transform& uvTransform);


	static Sprite* Create(
	    const std::string& fileName /* Vector4 color = {1, 1, 1, 1},
	    Vector2 anchorpoint = {0.0f, 0.0f}, bool isFlipX = false, bool isFlipY = false*/);


private: // メンバ変数
	TextureManager* textureManager_;
	uint32_t texture_;


	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce;
	// データを書き込む
	TransformationMatrix* wvpData;


	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	VertexData* vertexData_;

	
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	uint32_t* indexData_;

	Material* materialDataSprite;
	DirectionalLight* directionalLightData = nullptr;
	TransformationMatrix* transformationMatrixDataSprite = nullptr;

	/*Transform* uvTransformSprite{
	    {1.0f, 1.0f, 1.0f},
	    {0.0f, 0.0f, 0.0f},
	    {0.0f, 0.0f, 0.0f},
	};*/
	

	
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_;
	D3D12_INDEX_BUFFER_VIEW ibView_;
	// 頂点
	
	// ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> lightResource_;
	//座標
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite_; 

	// マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;
	
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// 定数バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> constBuff_;
	// 頂点バッファマップ
	VertexPosUv* vertMap_ = nullptr;
	// 定数バッファマップ
	ConstBufferData* constMap_ = nullptr;
	// テクスチャ番号
	UINT textureHandle_ = 0;
	// Z軸回りの回転角
	float rotation_ = 0.0f;
	// 座標
	Vector2 position_{};
	// スプライト幅、高さ
	Vector2 size_ = {1.0f, 1.0f};
	// アンカーポイント
	Vector2 anchorPoint_ = {0, 0};
	// ワールド行列
	Matrix4x4 matWorld_{};
	
	

private: // メンバ関数
	/// <summary>
	/// 頂点データ転送
	/// </summary>
	

	void CreateVertexResource();
	void LoadTexture(const std::string& fileName);
};
