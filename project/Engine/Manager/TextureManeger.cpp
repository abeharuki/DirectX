#include "TextureManeger.h"
#include <fstream>
#include <sstream>
#include <filesystem>

#pragma comment(lib, "dxcompiler.lib")


TextureManager* TextureManager::instance_ = nullptr;

void TextureManager::Initialize()
{
	descriptorSizeDSV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	descriptorSizeRTV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	descriptorSizeSRV = Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);
}

TextureManager* TextureManager::GetInstance() {
	if (instance_ == nullptr)
	{
		instance_ = new TextureManager();
	}
	return instance_;
}
void TextureManager::Destroy()
{
	if (instance_)
	{
		delete instance_;
		instance_ = nullptr;
	}
}

void TextureManager::Load(const std::string& filename) {
	TextureManager::GetInstance()->LoadInternal(filename);
}

uint32_t TextureManager::ParticleLoad(ID3D12Resource* pResource, uint32_t instanceCount) {

	textureIndex_++;
	CreateInstanceSRV(textureIndex_, pResource, instanceCount);
	return textureIndex_;
}

const D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetGPUHandle(uint32_t textureHandle) {
	//D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = textureSrvHandleGPU_[textureHandle];
	//return GPUHandle;

	assert(textureHandle < DirectXCommon::kMaxSRVCount);
	TextureData& textureData = textureDatas[textureHandle];
	return textureData.srvHandleGPU_;

}

const D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetParticleGPUHandle(uint32_t textureHandle) {
	D3D12_GPU_DESCRIPTOR_HANDLE GPUHandle = instancingSrvHandelGPU[textureHandle];
	return GPUHandle;
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath) {
	//読み込み済みテクスチャを検索
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);

	if (it != textureDatas.end()) {
		//読み込み済みなら要素番号を返す
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	assert(0);
	return 0;
}

// Textureデータの読み込み
DirectX::ScratchImage TextureManager::LoadTexture(const std::string& filePath) {

	// テクスチャファイルを読み込んでプログラムを扱えるようにする
	DirectX::ScratchImage image{};
	std::wstring filePathW = Utility::ConvertString(filePath);
	HRESULT hr{};
	if (filePathW.ends_with(L".dds")) {
		DirectX::LoadFromDDSFile(filePathW.c_str(), DirectX::DDS_FLAGS_NONE, nullptr, image);
	}
	else {
		DirectX::LoadFromWICFile(filePathW.c_str(), DirectX::WIC_FLAGS_FORCE_SRGB, nullptr, image);
	}
		
	assert(SUCCEEDED(hr));

	// ミニマップの作成
	DirectX::ScratchImage mipImages{};
	if (DirectX::IsCompressed(image.GetMetadata().format)) {
		mipImages = std::move(image);
	}
	else {
		hr = DirectX::GenerateMipMaps(
			image.GetImages(), image.GetImageCount(), image.GetMetadata(), DirectX::TEX_FILTER_SRGB, 4,
			mipImages);
	}
	

	// ミニマップ付きのデータを返す
	return mipImages;
};


void TextureManager::LoadInternal(const std::string& filePath) {
	//読み込み済みテクスチャを検索
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);

	if (it != textureDatas.end()) {
		//読み込み済みなら早期return
		return;
	}
	assert(textureDatas.size() + 1 < DirectXCommon::kMaxSRVCount);

	DirectX::ScratchImage mipImage = LoadTexture(filePath);
	//テクスチャーデータの追加
	textureDatas.resize(textureDatas.size() + 1);
	//追加したテクスチャーサイズの参照を取得
	TextureData& textureData = textureDatas.back();

	textureData.filePath = filePath;
	textureData.metadata = mipImage.GetMetadata();
	textureData.resource = CreateTextureResource(Engine::GetDevice().Get(), textureData.metadata);
	UploadTextureData(textureData.resource.Get(), mipImage);

	//テクスチャーデータの要素数番号をSRVのインデックスとする
	//uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop_;

	DescriptorHandle srvHandle_ = {};
	srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	textureData.srvHandleCPU_ = srvHandle_;//Engine::GetCPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, srvIndex);
	textureData.srvHandleGPU_ = srvHandle_;//Engine::GetGPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, srvIndex);
	

	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	if (textureData.metadata.IsCubemap()) {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MostDetailedMip = 0;
		srvDesc.TextureCube.MipLevels = UINT_MAX;
		srvDesc.TextureCube.ResourceMinLODClamp = 0.0f;
	}
	else {
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
		srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	}
	Engine::GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU_);

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
/*
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
*/

void TextureManager::CreateInstanceSRV(uint32_t index, ID3D12Resource* pResource, uint32_t instanceCount) {


	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceCount;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	DescriptorHandle srvHandle_ = {};
	srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	instancingSrvHandelCPU[index] = srvHandle_;
	instancingSrvHandelGPU[index] = srvHandle_;
	Engine::GetDevice()->CreateShaderResourceView(pResource, &instancingSrvDesc, instancingSrvHandelCPU[index]);
}


const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{

	assert(textureIndex < DirectXCommon::kMaxSRVCount);
	TextureData& textureData = textureDatas.back();
	return textureData.metadata;

	//return metadata[textureIndex];
}

void TextureManager::UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {
	// Meta情報を取得
	const DirectX::TexMetadata& metadata = mipImages.GetMetadata();
	// 全MipMapと全アレイ要素について
	for (size_t arraySlice = 0; arraySlice < metadata.arraySize; ++arraySlice) {
		for (size_t mipLevel = 0; mipLevel < metadata.mipLevels; ++mipLevel) {
			// MipMapLevelとアレイ要素を指定して各Imageを取得
			const DirectX::Image* img = mipImages.GetImage(mipLevel, arraySlice, 0);
			if (img) {
				// Textureに転送
				HRESULT hr = texture->WriteToSubresource(
					static_cast<UINT>(mipLevel + arraySlice * metadata.mipLevels),
					nullptr,
					img->pixels,
					static_cast<UINT>(img->rowPitch),
					static_cast<UINT>(img->slicePitch)
				);
				assert(SUCCEEDED(hr));
			}
		}
	}
}











