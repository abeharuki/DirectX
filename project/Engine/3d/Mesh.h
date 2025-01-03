#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "Math/math.h"
#include "WinApp.h"
#include <ColorBuffer.h>
#include <RWStructuredBuffer.h>
#include <StructuredBuffer.h>
#include <UploadBuffer.h>

/**
 * @file Mesh
 * @brief 3Dメッシュデータを管理するクラス
 */
class Mesh
{
public:
	//初期化
	void Initialize(const MeshData& meshData, const bool skinFlag);

	/*----------------ゲッター----------------*/
	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView; };
	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView; };
	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };
	const size_t GetIndicesSize() const { return meshData_.indices.size(); };
	const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

	RWStructuredBuffer* GetOutputVertex() const { return outputVertices_.get(); };
	StructuredBuffer* GetInputVertex() const { return inputVertices_.get(); };
	UploadBuffer* GetSkinningInformationBuffer() const { return skinningInformationBuffer_.get(); };

	//バッファーの作成
	//読み込み用リソースの作成
	static ID3D12Resource* CreateBufferResoure(ID3D12Device* device, size_t sizeInBytes);

private:
	//頂点バッファを作成
	void CreateVertexBuffer(bool flag);

	//インデックスバッファを作成
	void CreateIndexBuffer();

	//頂点入力用バッファを作成
	void CreateInputVerticesBuffer();

	//頂点出力用バッファを作成
	void CreateOutputVerticesBuffer();

	//頂点バッファビューを作成
	void CreateVertexBufferView();

	//スキニング用のバッファを作成
	void CreateSkinningBuffer();

private:
	MeshData meshData_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	std::unique_ptr<RWStructuredBuffer> outputVertices_ = nullptr;
	std::unique_ptr<StructuredBuffer> inputVertices_ = nullptr;
	
	std::unique_ptr<UploadBuffer> vertexBuffer_ = nullptr;
	std::unique_ptr<UploadBuffer> indexBuffer_ = nullptr;
	std::unique_ptr<UploadBuffer> skinningInformationBuffer_ = nullptr;
};

