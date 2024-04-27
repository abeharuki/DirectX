#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <array>
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>
#include <DescriptorHeap.h>
#include <Math/math.h>


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")



class RenderTexture {
public:

	//レンダー関連
	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();


	/// <summary>
	/// レンダーテクスチャの作成
	/// </summary>
	void CreateRenderTexture();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();

private:

	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;


	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource;
	D3D12_RESOURCE_BARRIER renderBarrier{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	D3D12_CPU_DESCRIPTOR_HANDLE srvHandle_;
};
