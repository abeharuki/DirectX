#include "Mesh.h"
#include "Engine.h"



void Mesh::Initialize(const MeshData& meshData)
{
	//頂点データの初期化
	meshData_ = meshData;

	//頂点バッファの作成
	CreateVertexBuffer();

	//インデックスバッファの作成
	if (!meshData_.indices.empty()) {
		CreateIndexBuffer();
	}
	

	//更新
	//Update();
}

void Mesh::Update()
{
}

void Mesh::CreateVertexBuffer()
{
	VertexData* vertexData = nullptr;
	// 頂点リソースを作る
	vertexResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), sizeof(VertexData) * meshData_.vertices.size());
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData)); // 書き込むためのアドレスを取得

	// 頂点バッファビューを作成する
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(
		sizeof(VertexData) * meshData_.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ


	std::memcpy(vertexData,meshData_.vertices.data(), sizeof(VertexData) * meshData_.vertices.size()); // 頂点データをリソースにコピース


}

void Mesh::CreateIndexBuffer()
{
	
	indexResource = CreateBufferResoure(Engine::GetDevice().Get(), sizeof(uint32_t) * meshData_.indices.size());

	// 頂点バッファビューを作成する
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	indexBufferView.SizeInBytes = sizeof(uint32_t) * UINT(meshData_.indices.size()); // 使用するリソースのサイズは頂点サイズ
	indexBufferView.Format = DXGI_FORMAT_R32_UINT; // 1頂点あたりのサイズ

	uint32_t* indexData = nullptr;
	indexResource->Map(0,nullptr, reinterpret_cast<void**>(&indexData)); // 書き込むためのアドレスを取得
	std::memcpy(indexData,meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size()); // 頂点データをリ

}

ID3D12Resource* Mesh::CreateBufferResoure(ID3D12Device* device, size_t sizeInBytes) {

	// 頂点リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES uploadHeapProperties{};
	uploadHeapProperties.Type = D3D12_HEAP_TYPE_UPLOAD; // UploadHeapを使う
	// 頂点リソースの設定
	D3D12_RESOURCE_DESC vertexResourceDesc{};
	// バッファリソース。テクスチャの場合はまた別の設定をする
	vertexResourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	vertexResourceDesc.Width = sizeInBytes;
	// バッファの場合はこれらは1にする決まり
	vertexResourceDesc.Height = 1;
	vertexResourceDesc.DepthOrArraySize = 1;
	vertexResourceDesc.MipLevels = 1;
	vertexResourceDesc.SampleDesc.Count = 1;
	// バッファの場合はこれにする決まり
	vertexResourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	ID3D12Resource* vertexResource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
	    &uploadHeapProperties, D3D12_HEAP_FLAG_NONE, &vertexResourceDesc,
	    D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&vertexResource));
	assert(SUCCEEDED(hr));

	return vertexResource;
};

