#include "TextureManeger.h"

#pragma comment(lib, "dxcompiler.lib")

void TextureManager::Initialize()
{
	
	descriptorSizeDSV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	descriptorSizeRTV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeSRV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	
}


TextureManager* TextureManager::GetInstance() {
	static TextureManager instance;
	return &instance;
}

uint32_t TextureManager::Load(const std::string& filePath) {
	
	textureIndex_++;
	LoadTexture(filePath, textureIndex_);
	return textureIndex_;
}

uint32_t TextureManager::ParticleLoad(ID3D12Resource* pResource, uint32_t instanceCount) {

	textureIndex_++;
	CreateInstanceSRV(textureIndex_, pResource, instanceCount);
	return textureIndex_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUHandle(uint32_t textureHandle) {
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = textureSrvHandleGPU_[textureHandle];
	return GPUHandle;
}

const D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetParticleGPUHandle(uint32_t textureHandle) {
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = instancingSrvHandelGPU[textureHandle];
	return GPUHandle;
}

// Textureデータの読み込み
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {
	// テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = Utility::ConvertString(filePath);
	HRESULT hr =
	    DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
	    image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 0,
	    mipImages);

	// ミニマップ付きのデータを返す
	return mipImages;
};


void TextureManager::LoadTexture(const std::string& filePath, uint32_t index) {

	DirectX::ScratchImage mipImage = LoadTexture(filePath);
	metadata[index] = mipImage.GetMetadata();
	textureResource[index] = CreateTextureResource(Engine::GetDevice().Get(), metadata[index]);
	UploadTextureData(textureResource[index].Get(), mipImage);
	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = metadata[index].format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(metadata[index].mipLevels);

	// SRVを作成するDescripterHeapの場所を決める
	textureSrvHandleGPU_[index] = Engine::GetGPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV,index+1); // direct_->GetSrvHeap()->GetGPUDescriptorHandleForHeapStart();
	textureSrvHandleCPU_[index] = Engine::GetCPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV,index+1);
	// 先頭はIMGUIが使ってるからその次を使う
	textureSrvHandleCPU_[index].ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureSrvHandleGPU_[index].ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Engine::GetDevice()->CreateShaderResourceView(textureResource[index].Get(), &srvDesc, textureSrvHandleCPU_[index]);
}


void TextureManager::CreateInstanceSRV(uint32_t index, ID3D12Resource* pResource,uint32_t instanceCount) {
	

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceCount;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandelCPU[index] =Engine::GetCPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, index+1);
	instancingSrvHandelGPU[index] =Engine::GetGPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, index+1);
	instancingSrvHandelCPU[index].ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize( D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	instancingSrvHandelGPU[index].ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	Engine::GetDevice()->CreateShaderResourceView(pResource, &instancingSrvDesc, instancingSrvHandelCPU[index]);
}


// TextureResourceの作成
ID3D12Resource* TextureManager::CreateTextureResource(
    ID3D12Device* device, const DirectX::TexMetadata& metadata) {
	// metadataを基にResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = UINT(metadata.width);                  // Textureの幅
	resourceDesc.Height = UINT(metadata.height);                // Textureの高さ
	resourceDesc.MipLevels = UINT16(metadata.mipLevels);        // mipmapの数
	resourceDesc.DepthOrArraySize = UINT16(metadata.arraySize); // 奥行き or 配列Textureの配列数
	resourceDesc.Format = metadata.format;                      // TextureのFormat
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension =
	    D3D12_RESOURCE_DIMENSION(metadata.dimension); // Textureの次元数。普段使ってるのは2次元

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_CUSTOM; // 細かい設定を行う
	heapProperties.CPUPageProperty =
	    D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
	    &heapProperties,                   // Heapの設定
	    D3D12_HEAP_FLAG_NONE,              // Heapの特殊な設定。特になし。
	    &resourceDesc,                     // Resourceの設定
	    D3D12_RESOURCE_STATE_GENERIC_READ, // 初回のResourceState。Textureは基本読むだけ
	    nullptr,
	    IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

// TextureResourceにデータ転送
void TextureManager::UploadTextureData(
    Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapについて
	for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
		// MipMapLevelを指定して各Imageを取得
		const DirectX::Image* img = mipImages.GetImage(mipLevel, 0, 0);
		// Textureに転送
		HRESULT hr = texture->WriteToSubresource(
		    UINT(mipLevel),
		    nullptr,              // 全領域へコピー
		    img->pixels,          // 元データアドレ
		    UINT(img->rowPitch),  // 1ランサイズ
		    UINT(img->slicePitch) // 1枚サイズ
		);
		assert(SUCCEEDED(hr));
	}
}

const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{
	//(textureIndex > 256);

	return metadata[textureIndex];
}







