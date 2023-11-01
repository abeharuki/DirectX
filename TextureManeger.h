#pragma once
#include "StringUtility.h"
#include "math.h"
#include "Engine.h"
#include <cassert>
#include <format>
#include <wrl.h>
#include <DirectXTex.h>
#include <d3dx12.h>

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
	
	//objファイルの読み込み
	static MaterialData LoadMaterialTemplateFile(const std::string& filename);
	static ModelData LoadObjFile(const std::string& filename);

	const D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(uint32_t textureHandle);

	static const int maxtex = 256;

	
private:
	
	void LoadTexture(const std::string& filePath, uint32_t index);


	Microsoft::WRL::ComPtr<ID3D12Resource> textureResource[maxtex];
	uint32_t descriptorSizeSRV;
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;
	D3D12_CPU_DESCRIPTOR_HANDLE textureSrvHandleCPU_[maxtex];
	D3D12_GPU_DESCRIPTOR_HANDLE textureSrvHandleGPU_[maxtex];
	//0がImGui1がパーティクルで使ってるから
	uint32_t textureIndex_;
};