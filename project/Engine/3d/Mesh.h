#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <unordered_map>
#include <vector>
#include <wrl.h>
#include "Math/math.h"
#include "WinApp.h"

class Mesh
{
public:

	void Initialize(const MeshData& meshData);

	void Update();

	const D3D12_VERTEX_BUFFER_VIEW& GetVertexBufferView() const { return vertexBufferView; };

	const D3D12_INDEX_BUFFER_VIEW& GetIndexBufferView() const { return indexBufferView; };

	const size_t GetVerticesSize() const { return meshData_.vertices.size(); };

	const size_t GetIndicesSize() const { return meshData_.indices.size(); };

	//const uint32_t GetMaterialIndex() const { return meshData_.materialIndex; };

	static ID3D12Resource* CreateBufferResoure(ID3D12Device* device, size_t sizeInBytes);


private:
	void CreateVertexBuffer();

	void CreateIndexBuffer();

private:
	MeshData meshData_{};

	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};
	// 頂点
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
};

