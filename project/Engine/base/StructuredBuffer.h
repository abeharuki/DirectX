#pragma once
#include "GpuResource.h"
#include "DescriptorHandle.h"
#include <cstdint>

/**
 * @class StructuredBuffer
 * @brief 構造体バッファ（読み取り専用）を管理するクラス
 */
class StructuredBuffer : public GpuResource
{
public:
    /// <summary>
    /// バッファの作成
    /// </summary>
    /// <param name="numElements">バッファ内の要素数</param>
    /// <param name="elementSize">1要素あたりのサイズ（バイト単位）</param>
    void Create(uint32_t numElements, uint32_t elementSize);

    /// <summary>
    /// バッファのデータをマップして、CPU側からアクセスできるようにする
    /// </summary>
    /// <returns>バッファのポインタ</returns>
    void* Map();

    /// <summary>
    /// バッファのマッピングを解除
    /// </summary>
    void Unmap();

    /// <summary>
    /// SRV（Shader Resource View）のディスクリプタハンドルを取得
    /// </summary>
    /// <returns>SRVのディスクリプタハンドル</returns>
    const DescriptorHandle& GetSRVHandle() { return srvHandle_; };

    /// <summary>
    /// バッファのサイズを取得
    /// </summary>
    /// <returns>バッファのサイズ（バイト単位）</returns>
    size_t GetBufferSize() const { return bufferSize_; };

private:
    /// <summary>
    /// SRV（Shader Resource View）を作成するための派生ビューの生成
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
    /// バッファのサイズ（バイト単位）
    /// </summary>
    size_t bufferSize_ = 0;

    /// <summary>
    /// バッファ内の要素数
    /// </summary>
    uint32_t elementCount_ = 0;

    /// <summary>
    /// 1要素あたりのサイズ（バイト単位）
    /// </summary>
    uint32_t elementSize_ = 0;
};

