#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

/**
 * @class RWStructuredBuffer
 * @brief 領域を読み書き可能な構造体バッファを管理するクラス
 */
class RWStructuredBuffer : public GpuResource
{
public:
    /// <summary>
    /// バッファの作成
    /// </summary>
    /// <param name="numElements">バッファ内の要素数</param>
    /// <param name="elementSize">1要素あたりのサイズ（バイト単位）</param>
    void Create(uint32_t numElements, uint32_t elementSize);

    /// <summary>
    /// SRV（Shader Resource View）のディスクリプタハンドルを取得
    /// </summary>
    /// <returns>SRVのディスクリプタハンドル</returns>
    const DescriptorHandle& GetSRVHandle() const { return srvHandle_; };

    /// <summary>
    /// UAV（Unordered Access View）のディスクリプタハンドルを取得
    /// </summary>
    /// <returns>UAVのディスクリプタハンドル</returns>
    const DescriptorHandle& GetUAVHandle() { return uavHandle_; };

    /// <summary>
    /// バッファのサイズを取得
    /// </summary>
    /// <returns>バッファのサイズ（バイト単位）</returns>
    size_t GetBufferSize() const { return bufferSize_; };

private:
    /// <summary>
    /// SRV と UAV を生成するための派生ビューの作成
    /// </summary>
    /// <param name="device">Direct3D 12 デバイス</param>
    /// <param name="numElements">要素数</param>
    /// <param name="elementSize">1要素あたりのサイズ（バイト単位）</param>
    void CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize);

private:
    /// <summary>
    /// SRV（Shader Resource View）のディスクリプタハンドル
    /// </summary>
    DescriptorHandle srvHandle_{};

    /// <summary>
    /// UAV（Unordered Access View）のディスクリプタハンドル
    /// </summary>
    DescriptorHandle uavHandle_{};

    /// <summary>
    /// バッファのサイズ（バイト単位）
    /// </summary>
	size_t bufferSize_ = 0;
};

