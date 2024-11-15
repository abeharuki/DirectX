#include "Mesh.h"
#include "Engine.h"




void Mesh::Initialize(const MeshData& meshData, const bool skinFlag)
{
	//頂点データの初期化
	meshData_ = meshData;

	//頂点バッファの作成
	CreateVertexBuffer(skinFlag);

	//インデックスバッファの作成
	if (!meshData_.indices.empty()) {
		CreateIndexBuffer();
	}

}


void Mesh::CreateVertexBuffer(bool flag)
{


	// 頂点バッファビューを作成する
	if (flag) {

		CreateInputVerticesBuffer();
		CreateOutputVerticesBuffer();
		CreateVertexBufferView();
		CreateSkinningBuffer();

	}
	else {


		// 頂点リソースを作る
		vertexBuffer_ = std::make_unique<UploadBuffer>();
		vertexBuffer_->Create(sizeof(VertexData) * meshData_.vertices.size());

		vertexBufferView.BufferLocation = vertexBuffer_->GetGpuVirtualAddress(); // リソースの先頭のアドレスから使う
		vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * meshData_.vertices.size()); // 使用するリソースのサイズは頂点サイズ
		vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ

		VertexData* vertexData = static_cast<VertexData*>(vertexBuffer_->Map());
		std::memcpy(vertexData, meshData_.vertices.data(), sizeof(VertexData) * meshData_.vertices.size()); // 頂点データをリソースにコピース
		vertexBuffer_->Unmap();


	}



}

void Mesh::CreateIndexBuffer()
{

	//インデックスバッファの作成
	indexBuffer_ = std::make_unique<UploadBuffer>();
	indexBuffer_->Create(sizeof(uint32_t) * meshData_.indices.size());

	//インデックスバッファビューの作成
	indexBufferView.BufferLocation = indexBuffer_->GetGpuVirtualAddress();
	indexBufferView.SizeInBytes = static_cast<UINT>(sizeof(uint32_t) * meshData_.indices.size());
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;

	//インデックスバッファにデータを書き込む
	uint32_t* indexData = static_cast<uint32_t*>(indexBuffer_->Map());
	std::memcpy(indexData, meshData_.indices.data(), sizeof(uint32_t) * meshData_.indices.size());
	indexBuffer_->Unmap();
}

void Mesh::CreateInputVerticesBuffer() {
	//InputVerticeBufferの作成
	inputVertices_ = std::make_unique<StructuredBuffer>();
	inputVertices_->Create((uint32_t)meshData_.vertices.size(), sizeof(VertexData));
	VertexData* inputVertexData = static_cast<VertexData*>(inputVertices_->Map());
	std::memcpy(inputVertexData, meshData_.vertices.data(), sizeof(VertexData) * meshData_.vertices.size());
	inputVertices_->Unmap();
}

void Mesh::CreateOutputVerticesBuffer()
{
	//UAVResourceの作成
	outputVertices_ = std::make_unique<RWStructuredBuffer>();
	outputVertices_->Create((uint32_t)meshData_.vertices.size(), sizeof(VertexData));
}

void Mesh::CreateVertexBufferView() {
	vertexBufferView.BufferLocation = outputVertices_->GetResource()->GetGPUVirtualAddress(); // リソースの先頭のアドレスから使う
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * meshData_.vertices.size()); // 使用するリソースのサイズは頂点サイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData); // 1頂点あたりのサイズ
}

void Mesh::CreateSkinningBuffer() {
	//SkinningInformationBufferの作成
	skinningInformationBuffer_ = std::make_unique<UploadBuffer>();
	skinningInformationBuffer_->Create(sizeof(uint32_t));

	//スキンニング情報を書き込む
	uint32_t* skinningInformationData = static_cast<uint32_t*>(skinningInformationBuffer_->Map());
	*skinningInformationData = static_cast<uint32_t>(meshData_.vertices.size());
	skinningInformationBuffer_->Unmap();
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
	hr;
	return vertexResource;
};