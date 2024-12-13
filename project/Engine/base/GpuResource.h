#pragma once
#include <d3d12.h>
#include <wrl.h>

/**
 * @class GpuResource
 * @brief GPUリソースを管理する基底クラス
 */
class GpuResource
{
    /// <summary>
    /// CommandContext クラスからアクセス可能にするためのフレンドクラス宣言
    /// </summary>
    friend class CommandContext;

public:
    /// <summary>
    /// デストラクタ
    /// </summary>
    virtual ~GpuResource() = default;

    /// <summary>
    /// GPUリソースを取得
    /// </summary>
    /// <returns>ID3D12Resource ポインタ</returns>
    ID3D12Resource* GetResource() const { return resource_.Get(); };

    /// <summary>
    /// GPU仮想アドレスを取得
    /// </summary>
    /// <returns>D3D12_GPU_VIRTUAL_ADDRESS</returns>
    D3D12_GPU_VIRTUAL_ADDRESS GetGpuVirtualAddress() const { return gpuVirtualAddress_; };

    /// <summary>
    /// 現在のリソースステートを取得
    /// </summary>
    /// <returns>D3D12_RESOURCE_STATES</returns>
    D3D12_RESOURCE_STATES GetResourceState() { return currentState_; };

    /// <summary>
    /// リソースステートを設定
    /// </summary>
    /// <param name="newState">新しいリソースステート</param>
    void SetResourceState(D3D12_RESOURCE_STATES newState) { currentState_ = newState; };

    // リソース設定用の関数をコメントアウト
    //void SetResource(ID3D12Resource* resource) { resource_ = resource; }

protected:
    /// <summary>
    /// GPUリソースを保持するための ComPtr
    /// </summary>
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_ = nullptr;

    /// <summary>
    /// 現在のリソースのステート
    /// </summary>
    D3D12_RESOURCE_STATES currentState_{};  ///< リソースの現在の状態（例: D3D12_RESOURCE_STATE_COMMON）

    /// <summary>
    /// GPU仮想アドレス
    /// </summary>
    D3D12_GPU_VIRTUAL_ADDRESS gpuVirtualAddress_ = 0;  ///< GPUの仮想メモリアドレス
};