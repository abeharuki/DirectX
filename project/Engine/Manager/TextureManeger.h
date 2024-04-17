#pragma once
#include "StringUtility.h"
#include "Math/math.h"
#include "Engine.h"
#include <cassert>
#include <winerror.h>
#include <combaseapi.h>
#include <dxcapi.h>
#include <format>
#include <cstdlib>
#include <d3d12.h>
#include <wrl.h>
#include <fstream>
#include <sstream>
#include <filesystem>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <DirectXTex.h>



class TextureManager {
public:
	
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static TextureManager* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();
	

	uint32_t Load(const std::string& fileName);

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	static void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
	const DirectX::TexMetadata& GetMetaData(uint32_t textureIndex);

	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t textureHandle);

	uint32_t ParticleLoad(ID3D12Resource* pResource, uint32_t instanceCount);
	const D3D12_GPU_DESCRIPTOR_HANDLE GetParticleGPUHandle(uint32_t textureHandle);



	static const int maxtex = 256;

	
private:
	
	void LoadTexture(const std::string& filePath, uint32_t index);

	void CreateInstanceSRV(uint32_t index, ID3D12Resource* pResource, uint32_t instanceCount);



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