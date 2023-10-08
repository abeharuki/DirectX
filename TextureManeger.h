#pragma once
#include "StringUtility.h"
#include "ResourceObject.h"
#include <wrl.h>
#include "externals/DirectXTex/DirectXTex.h"
#include "externals/DirectXTex/d3dx12.h"

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

	static DirectX::ScratchImage LoadTexture(const std::string& filePath);
	static ID3D12Resource* CreateTextureResource(ID3D12Device* device, const DirectX::TexMetadata& metadata);
	static void UploadTextureData(Microsoft::WRL::ComPtr<ID3D12Resource> texture, const DirectX::ScratchImage& mipImages);
	
	
private:
	
	Utility* utility_;
};