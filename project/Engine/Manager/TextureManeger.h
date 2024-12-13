#pragma once
#include "StringUtility.h"
#include "Math/math.h"
#include "Engine.h"
#include <DirectXTex.h>

/**
 * @class TextureManager
 * @brief テクスチャの読み込みと管理を行うクラス
 */
class TextureManager {
public:
	
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	static void Destroy();

	//読み込み検索
	static void Load(const std::string& filename);

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	static void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

	

	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t textureHandle);
	//SRVインデックスの開始番号
	uint32_t GetTextureIndexByFilePath(const std::string& filename);

	//パーティクル用
	uint32_t ParticleLoad(ID3D12Resource* pResource, uint32_t instanceCount);
	const D3D12_GPU_DESCRIPTOR_HANDLE GetParticleGPUHandle(uint32_t textureHandle);


	static const int maxtex = 512;

private:

	//構造体
	struct TextureData {
		std::string filePath;
		DirectX::TexMetadata metadata;
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU_;
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU_;
	};


private:

	//void LoadTexture(const std::string& filePath, uint32_t index);
	void LoadInternal(const std::string& filePath);

	void CreateInstanceSRV(uint32_t index, ID3D12Resource* pResource, uint32_t instanceCount);

	static TextureManager* instance_;
	static uint32_t kSRVIndexTop_;

	std::vector<TextureData>textureDatas;

	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[maxtex];
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_[maxtex];
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_[maxtex];
	//particle
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandelCPU[maxtex];
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandelGPU[maxtex];
	DirectX::TexMetadata metadata[maxtex];
	//0がImGui1がパーティクルで使ってるから
	uint32_t textureIndex_;
};