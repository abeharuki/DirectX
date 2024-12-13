#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

/**
 * @class ColorBuffer
 * @brief GPUリソースとしての色バッファを管理するクラス
 */
class ColorBuffer : public GpuResource
{
public:
	/// <summary>
	/// 色バッファを作成
    /// </summary>
    /// <param name="baseResource">ベースとなるリソース（スワップチェーン）</param>
	void CreateFromSwapChain(ID3D12Resource* baseResource);

	/// <summary>
	/// 幅と高さを指定して色バッファを作成
	/// </summary>
	/// <param name="width">バッファの幅</param>
	/// <param name="height">バッファの高さ</param>
	/// <param name="format">バッファの形式</param>
	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format);

	/// <summary>
	/// 幅、高さ、形式、クリアカラーを指定して色バッファを作成
	/// </summary>
	/// <param name="width">バッファの幅</param>
	/// <param name="height">バッファの高さ</param>
	/// <param name="format">バッファの形式</param>
	/// <param name="clearColor">クリアカラー（RGBA）</param>
	void Create(uint32_t width, uint32_t height, DXGI_FORMAT format, float* clearColor);

	/// <summary>
	/// レンダーターゲットビュー(RTV)のハンドルを取得
	/// </summary>
	/// <returns>RTVハンドル</returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVHandle() const { return rtvHandle_; };

	/// <summary>
	/// シェーダーリソースビュー(SRV)のハンドルを取得
	/// </summary>
	/// <returns>SRVハンドル</returns>
	const DescriptorHandle& GetSRVHandle() const { return srvHandle_; };

	/// <summary>
	/// クリアカラーを取得
	/// </summary>
	/// <returns>クリアカラー（RGBA）の配列</returns>
	const float* GetClearColor() const { return clearColor_; };

private:
	/// <summary>
	/// デバイスと形式を指定して、関連するビューを作成
	/// </summary>
	/// <param name="device">デバイス</param>
	/// <param name="format">バッファの形式</param>
	void CreateDerivedViews(ID3D12Device* device, DXGI_FORMAT format);

private:
	DescriptorHandle srvHandle_{};

	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_{};

	float clearColor_[4]{ 0.1f, 0.25f, 0.5f, 1.0f };
};

