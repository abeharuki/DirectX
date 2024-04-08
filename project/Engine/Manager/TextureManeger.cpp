#include "TextureManeger.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <d3dx12.h>
#pragma comment(lib, "dxcompiler.lib")

TextureManager* TextureManager::instance_ = nullptr;
uint32_t TextureManager::kSRVIndexTop_ = 1;

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
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop_;


	textureData.srvHandleCPU_ = Engine::GetCPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, srvIndex);
	textureData.srvHandleGPU_ = Engine::GetGPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, srvIndex);
	//textureData.srvHandleCPU_.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//textureData.srvHandleGPU_.ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);


	// metaDataを元にSRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D; // 2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	Engine::GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU_);

}

void TextureManager::CreateInstanceSRV(uint32_t index, ID3D12Resource* pResource, uint32_t instanceCount) {


	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};
	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = instanceCount;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	instancingSrvHandelCPU[index] = Engine::GetCPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, index + 100);
	instancingSrvHandelGPU[index] = Engine::GetGPUDescriptorHandle(Engine::GetSRV().Get(), descriptorSizeSRV, index + 100);
	instancingSrvHandelCPU[index].ptr += Engine::GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
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
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // 細かい設定を行う
	//heapProperties.CPUPageProperty =D3D12_CPU_PAGE_PROPERTY_WRITE_BACK; // WriteBackポリシーでCPUアクセス可能
	//heapProperties.MemoryPoolPreference = D3D12_MEMORY_POOL_L0; // プロセッサの近くに配置

	// Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,                   // Heapの設定
		D3D12_HEAP_FLAG_NONE,              // Heapの特殊な設定。特になし。
		&resourceDesc,                     // Resourceの設定
		D3D12_RESOURCE_STATE_COPY_DEST, // 初回のResourceState。Textureは基本読むだけ
		nullptr,
		IID_PPV_ARGS(&resource)); // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

// TextureResourceにデータ転送
[[nodiscard]]
void TextureManager::UploadTextureData(
	Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages) {
	/*
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
	}*/
	
	ID3D12GraphicsCommandList* commandList = Engine::GetList().Get();
	ID3D12CommandQueue* commandQueue = Engine::GetQueue().Get();
	ID3D12CommandAllocator* commandAllocator = Engine::GetAllocator().Get();

	std::vector<D3D12_SUBRESOURCE_DATA> subresourses;
	DirectX::PrepareUpload(Engine::GetDevice().Get(), mipImages.GetImages(), mipImages.GetImageCount(), mipImages.GetMetadata(), subresourses);
	uint64_t intermediateSize = GetRequiredIntermediateSize(texture.Get(), 0, UINT(subresourses.size()));

	ID3D12Resource* intermediateResource = Mesh::CreateBufferResoure(Engine::GetDevice().Get(), intermediateSize);
	UpdateSubresources(commandList, texture.Get(), intermediateResource, 0, 0, UINT(subresourses.size()), subresourses.data());

	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = texture.Get();
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_GENERIC_READ;
	commandList->ResourceBarrier(1, &barrier);
	
	commandList->Close();
	ID3D12CommandList* commandLists[] = { commandList };
	commandQueue->ExecuteCommandLists(1, commandLists);

	// フェンスを作成する
	// フェンスをシグナルする
	commandQueue->Signal(fence_.Get(), ++fenceVal_);

	// フェンスがシグナルされるまで待機する
	if (fence_->GetCompletedValue() < fenceVal_) {
		HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent);
		WaitForSingleObject(fenceEvent, INFINITE);
		CloseHandle(fenceEvent);
	}

	HRESULT hr = S_FALSE;
	// 次のフレーム用のコマンドリストを準備
	hr = commandAllocator->Reset();
	assert(SUCCEEDED(hr));
	hr = commandList->Reset(commandAllocator, nullptr);
	assert(SUCCEEDED(hr));
	
}

const DirectX::TexMetadata& TextureManager::GetMetaData(uint32_t textureIndex)
{

	assert(textureIndex < DirectXCommon::kMaxSRVCount);
	TextureData& textureData = textureDatas.back();
	return textureData.metadata;

	//return metadata[textureIndex];
}



MaterialData TextureManager::LoadMaterialTemplateFile(const std::string& filename) {
	// 宣言
	MaterialData materialData; // 構築するMaterialData
	ModelData modelData;
	std::string line;                              // ファイルから読んだ1行を格納する
	std::ifstream file(filename); // ファイルを開く
	assert(file.is_open());                        // 開けなかったら止める

	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();


	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identifierに応じた処理
		if (identifier == "map_Kd") {
			std::string textureFilename;
			s >> textureFilename;
			// 連結しているファイルパス
			materialData.textureFilePath = directryPath + "/" + textureFilename;
		} //else if (identifier == "mtllib") {
		//	// materialTemplateLibraryファイルの名前を取得する
		//	std::string materialFilename;
		//	s >> materialFilename;

		//	// 基本的にobjファイルと同じ一階層にmtlは存在させるので、ディレクション名などファイル名を渡す
		//	modelData.material = LoadMaterialTemplateFile(materialFilename);
		//}
	}

	return materialData;
}



ModelData TextureManager::LoadObjFile(const std::string& filename) {
	// 必要な変数の宣言
	ModelData modelData;            // 構築するModelData
	std::vector<Vector4> positions; // 位置
	std::vector<Vector3> normals;   // 法線
	std::vector<Vector2> texcoords; // テクスチャ座標
	std::string line;               // ファイルから選んだ１行を格納する

	// ファイルを開く
	std::ifstream file(filename);
	assert(file.is_open()); // とりあえず開けなかったら止める

	std::filesystem::path ps = std::filesystem::path(filename);
	std::string directryPath = ps.parent_path().string();

	// ファイルを読み,ModelDataを構築
	while (std::getline(file, line)) {
		std::string identifier;
		std::istringstream s(line);
		s >> identifier; // 先頭の識別子を読む

		// identfierの応じた処理
		// 頂点情報を読み込む
		if (identifier == "v") {
			Vector4 position = { 0.0f, 0.0f, 0.0f, 0.0f };
			s >> position.x >> position.y >> position.z;
			position.z *= -1.0f; // 位置の反転
			position.w = 1.0f;
			positions.push_back(position);
		}
		else if (identifier == "vt") {
			Vector2 texcoord = { 0.0f, 0.0f };
			s >> texcoord.x >> texcoord.y;
			texcoord.y = 1.0f - texcoord.y;
			texcoords.push_back(texcoord);
		}
		else if (identifier == "vn") {
			Vector3 normal = { 0.0f, 0.0f, 0.0f };
			normal.z *= -1.0f; // 法線の反転
			s >> normal.x >> normal.y >> normal.z;
			normals.push_back(normal);
		}
		else if (identifier == "f") {
			VertexData triangle[3];
			// 面は三角形減退。その他は未対応
			for (int32_t faceVertex = 0; faceVertex < 3; ++faceVertex) {
				std::string vertexDefinition;
				s >> vertexDefinition;
				// 頂点の要素へのIndexは[位置/UV/法線]で格納しているので、分解してIndexを取得する
				std::istringstream v(vertexDefinition);
				uint32_t elementIndices[3];
				for (int32_t element = 0; element < 3; ++element) {
					std::string index;
					std::getline(v, index, '/'); // 区切りでインデックスを読んでく
					elementIndices[element] = std::stoi(index);
				}

				// 要素へのIndexから、実際の要素の値を取得して、頂点を構築する
				Vector4 position = positions[elementIndices[0] - 1];
				Vector2 texcoord = texcoords[elementIndices[1] - 1];
				Vector3 normal = normals[elementIndices[2] - 1];
				// VertexData vertex = { position,texcoord,normal };
				// modelData.vertices.push_back(vertex);
				triangle[faceVertex] = { position, texcoord, normal };
			}

			modelData.vertices.push_back(triangle[2]);
			modelData.vertices.push_back(triangle[1]);
			modelData.vertices.push_back(triangle[0]);

		}
		else if (identifier == "mtllib") {
			// materialTemplateLibraryファイルの名前を取得
			std::string materialFilename;
			s >> materialFilename;
			materialFilename = directryPath + "/" + materialFilename;

			// 基本的にobjファイルと同一階層にmtlは存在させるから、ディレクトリ名とファイル名を渡す
			modelData.material = LoadMaterialTemplateFile(materialFilename);
		}
	}
	return modelData;
}
