#pragma once
#include "GpuResource.h"

/**
 * @class UploadBuffer
 * @brief GPUへのアップロード用バッファを管理するクラス
 */
class UploadBuffer : public GpuResource
{
public:
    /// <summary>
    /// アップロードバッファを作成
    /// </summary>
    /// <param name="sizeInBytes">バッファのサイズ（バイト単位）</param>
    void Create(size_t sizeInBytes);

    /// <summary>
    /// バッファをマップして、CPUからのデータ書き込みを可能にする
    /// </summary>
    /// <returns>バッファのポインタ</returns>
    void* Map();

    /// <summary>
    /// バッファのマッピングを解除し、GPUにデータを転送できるようにする
    /// </summary>
    void Unmap();

    /// <summary>
    /// アップロードバッファのサイズを取得
    /// </summary>
    /// <returns>バッファのサイズ（バイト単位）</returns>
    size_t GetBufferSize() const { return bufferSize_; };

private:
    /// <summary>
    /// アップロードバッファのサイズ（バイト単位）
    /// </summary>
    size_t bufferSize_ = 0;
};