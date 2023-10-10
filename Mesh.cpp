#include "Mesh.h"
#include "Engine.h"


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


void Mesh::CreateBuffers(ID3D12Device* device) {
	// バーテックス用の頂点リソースを作る
	vertexResource_ = CreateBufferResoure(device, sizeof(VertexData) * 6);

	// インデックス用の頂点リソースを作る
	indexResource_ = CreateBufferResoure(device, sizeof(uint32_t) * 6);

	// WVP用のリソースを作る。Matrix4x4 1つ分のサイズを用意する
	wvpResouce_ = CreateBufferResoure(device, sizeof(TransformationMatrix));

	// マテリアル用のリソースを作る。今回はcolor1つ分のサイズを用意する
	materialResorce_ = CreateBufferResoure(device, sizeof(Material));

	// ライティング
	directionalLightResource_ = CreateBufferResoure(device, sizeof(DirectionalLight));

	// Sprite用の頂点リソースを作る
	//vertexResourceSprite_ = CreateBufferResoure(device, sizeof(VertexData) * 4);
	
	// 頂点バッファビューを作成する
	// リソースの先頭のアドレスから使う
	vbView_.BufferLocation = vertexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズは頂点3つ分のサイズ
	vbView_.SizeInBytes = sizeof(VertexData) * 6;
	// 1頂点あたりのサイズ
	vbView_.StrideInBytes = sizeof(VertexData);
    

	
	// 頂点バッファビューを作成する
	// リソースの先頭アドレスから使う
	ibView_.BufferLocation = indexResource_->GetGPUVirtualAddress();
	// 使用するリソースのサイズはインデックス6つ分のサイズ
	ibView_.SizeInBytes = sizeof(uint32_t) * 6;
	// インデックスはuint32_tとする
	ibView_.Format = DXGI_FORMAT_R32_UINT;
	



	// インデックスを使った四角形
	// インデックス用の頂点リソースを作る
	//indexResourceSprite_ = CreateBufferResoure(Engine::GetDevice(), sizeof(uint32_t) * 6);
	// 頂点バッファビューを作成する
	//D3D12_INDEX_BUFFER_VIEW indexBufferViewSprite{};
	//// リソースの先頭アドレスから使う
	//indexBufferViewSprite.BufferLocation = indexResourceSprite_->GetGPUVirtualAddress();
	//// 使用するリソースのサイズはインデックス6つ分のサイズ
	//indexBufferViewSprite.SizeInBytes = sizeof(uint32_t) * 6;
	//// インデックスはuint32_tとする
	//indexBufferViewSprite.Format = DXGI_FORMAT_R32_UINT;
	




	
	
};