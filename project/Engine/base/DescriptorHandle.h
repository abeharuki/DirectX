#pragma once
#include <d3d12.h>

/**
 * @class DescriptorHandle
 * @brief GPUのリソースに関連するディスクリプタハンドルを管理するクラス
 */
class DescriptorHandle
{
public:
	/// <summary>
	/// デフォルトコンストラクタ
	/// </summary>
	DescriptorHandle() = default;

	/// <summary>
	/// CPUディスクリプタハンドルとGPUディスクリプタハンドルを指定してコンストラクタを初期化
	/// </summary>
	/// <param name="cpuHandle">CPUディスクリプタハンドル</param>
	/// <param name="gpuHandle">GPUディスクリプタハンドル</param>
	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle, D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle)
		: cpuHandle_(cpuHandle), gpuHandle_(gpuHandle) {};

	/// <summary>
	/// CPUディスクリプタハンドルのみ指定してコンストラクタを初期化
	/// </summary>
	/// <param name="cpuHandle">CPUディスクリプタハンドル</param>
	DescriptorHandle(D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle) : cpuHandle_(cpuHandle) {};

	/// <summary>
	/// ディスクリプタサイズを加算する演算子
	/// </summary>
	/// <param name="descriptorSize">加算するディスクリプタのサイズ</param>
	void operator+=(UINT descriptorSize)
	{
		// CPUおよびGPUのディスクリプタハンドルに指定されたサイズを加算
		cpuHandle_.ptr += descriptorSize;
		gpuHandle_.ptr += descriptorSize;
	}

	/// <summary>
	/// CPUディスクリプタハンドルのポインタを取得
	/// </summary>
	/// <returns>CPUディスクリプタハンドルへのポインタ</returns>
	const D3D12_CPU_DESCRIPTOR_HANDLE* operator&() const { return &cpuHandle_; }

	/// <summary>
	/// CPUディスクリプタハンドルへのキャスト演算子
	/// </summary>
	/// <returns>CPUディスクリプタハンドル</returns>
	operator D3D12_CPU_DESCRIPTOR_HANDLE() const { return cpuHandle_; };

	/// <summary>
	/// GPUディスクリプタハンドルへのキャスト演算子
	/// </summary>
	/// <returns>GPUディスクリプタハンドル</returns>
	operator D3D12_GPU_DESCRIPTOR_HANDLE() const { return gpuHandle_; };

private:
	/// <summary>
	/// CPUディスクリプタハンドル
	/// </summary>
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle_{};

	/// <summary>
	/// GPUディスクリプタハンドル
	/// </summary>
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle_{};
};
