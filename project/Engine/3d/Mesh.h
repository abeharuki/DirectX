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

class Mesh
{
public:

	void Initialize(const ModelData& meshData);

	void Update();

	const Microsoft::WRL::ComPtr<ID3D12Resource>& GetVertexResource() const { return vertexResource; };

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView; };

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView; };

	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };

	const size_t GetIndicesSize() const { return meshData_.indices.size(); };

	const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

	
	//バッファーの作成
	//読み込み用リソースの作成
	static ID3D12Resource* CreateBufferResoure(ID3D12Device* device, size_t sizeInBytes);


	RWStructuredBuffer* GetOutputVertex() const { return outputVertices_.get(); };
	StructuredBuffer* GetInputVertex() const { return inputVertices_.get(); };


private:
	void CreateVertexBuffer();

	void CreateIndexBuffer();

private:
	ModelData modelData_{};
	MeshData meshData_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;

	std::unique_ptr<RWStructuredBuffer> outputVertices_ = nullptr;
	std::unique_ptr<StructuredBuffer> inputVertices_ = nullptr;
	
};

