#pragma once
#include <Windows.h>
#include <d3d12.h>

#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "Math.h"
#include "WinApp.h"

class Mesh {


public: // サブクラス
	// 頂点データ構造体（テクスチャあり）
	struct VertexPosNormalUv {
		Vector3 pos;    // xyz座標
		Vector3 normal; // 法線ベクトル
		Vector2 uv;     // uv座標
	};

public: // メンバ関数

	

	/// <summary>
	/// 名前を取得
	/// </summary>
	/// <returns>名前</returns>
	const std::string& GetName() const { return name_; }

	/// <summary>
	/// 名前をセット
	/// </summary>
	/// <param name="name">名前</param>
	void SetName(const std::string& name_);

	/// <summary>
	/// 頂点データの追加
	/// </summary>
	/// <param name="vertex">頂点データ</param>
	void AddVertex(const VertexPosNormalUv& vertex);

	/// <summary>
	/// 頂点インデックスの追加
	/// </summary>
	/// <param name="index">インデックス</param>
	void AddIndex(unsigned short index);

	/// <summary>
	/// 頂点データの数を取得
	/// </summary>
	/// <returns>頂点データの数</returns>
	inline size_t GetVertexCount() const { return vertices_.size(); }

	/// <summary>
	/// エッジ平滑化データの追加
	/// </summary>
	/// <param name="indexPosition">座標インデックス</param>
	/// <param name="indexVertex">頂点インデックス</param>
	void AddSmoothData(unsigned short indexPosition, unsigned short indexVertex);

	/// <summary>
	/// 平滑化された頂点法線の計算
	/// </summary>
	void CalculateSmoothedVertexNormals();

	
	
	/// <summary>
	/// バッファの生成
	/// </summary>
	void CreateBuffers(ID3D12Device* device);

	/// <summary>
	/// 頂点バッファ取得
	/// </summary>
	/// <returns>頂点バッファ</returns>
	const D3D12_VERTEX_BUFFER_VIEW& GetVBView() const { return vbView_; }
	

	/// <summary>
	/// インデックスバッファ取得
	/// </summary>
	/// <returns>インデックスバッファ</returns>
	const D3D12_INDEX_BUFFER_VIEW& GetIBView() const { return ibView_; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertex() const { return vertexResource_; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetIndex() const { return indexResource_; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetSprite() const { return vertexResourceSprite_; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetWVP() const { return wvpResouce_; }

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetMaterial() const { return materialResorce_; }
	
	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetLight() const {
		return directionalLightResource_;
	}

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetTransformSprite() const {
		return transformationMatrixResourceSprite_;
	}

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="commandList">命令発行先コマンドリスト</param>
	/// <param name="rooParameterIndexMaterial">マテリアルのルートパラメータ番号</param>
	/// <param name="rooParameterIndexTexture">テクスチャのルートパラメータ番号</param>
	void Draw(
	    ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
	    UINT rooParameterIndexTexture);

	/// <summary>
	/// 描画（テクスチャ差し替え版）
	/// </summary>
	/// <param name="commandList">命令発行先コマンドリスト</param>
	/// <param name="rooParameterIndexMaterial">マテリアルのルートパラメータ番号</param>
	/// <param name="rooParameterIndexTexture">テクスチャのルートパラメータ番号</param>
	/// <param name="textureHandle">差し替えるテクスチャハンドル</param>
	void Draw(
	    ID3D12GraphicsCommandList* commandList, UINT rooParameterIndexMaterial,
	    UINT rooParameterIndexTexture, uint32_t textureHandle);

	/// <summary>
	/// 頂点配列を取得
	/// </summary>
	/// <returns>頂点配列</returns>
	inline const std::vector<VertexPosNormalUv>& GetVertices() { return vertices_; }

	/// <summary>
	/// インデックス配列を取得
	/// </summary>
	/// <returns>インデックス配列</returns>
	inline const std::vector<unsigned short>& GetIndices() { return indices_; }

	
	static ID3D12Resource* CreateBufferResoure(ID3D12Device* device, size_t sizeInBytes);

private: // メンバ変数
	// ウィンドウズアプリケーション管理
	WinApp* winApp_;
	
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	// 名前
	std::string name_;
	// 頂点バッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> vertBuff_;
	// インデックスバッファ
	Microsoft::WRL::ComPtr<ID3D12Resource> indexBuff_;
	// 頂点バッファビュー
	D3D12_VERTEX_BUFFER_VIEW vbView_ = {};
	// 頂点バッファビューを作成する
	D3D12_VERTEX_BUFFER_VIEW vbViewSprite_ = {};
	// インデックスバッファビュー
	D3D12_INDEX_BUFFER_VIEW ibView_ = {};
	// 頂点データ配列
	std::vector<VertexPosNormalUv> vertices_;
	// 頂点インデックス配列
	std::vector<unsigned short> indices_;
	// 頂点法線スムージング用データ
	std::unordered_map<unsigned short, std::vector<unsigned short>> smoothData_;

	//頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource_;
	//インデックス
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource_;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResourceSprite_;
	// Sprite用の頂点リソースを作る
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResourceSprite_;
	//ライティング
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource_;
	//WVP用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> wvpResouce_;
	//マテリアル用リソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResorce_;

	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResourceSprite_; 

	Math* math_;
};
