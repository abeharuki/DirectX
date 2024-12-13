#pragma once
#include "DescriptorHandle.h"
#include <cstdint>
#include <wrl.h>

/**
 * @class DescriptorHeap
 * @brief Direct12ディスクリプタヒープを管理するクラス
 */
class DescriptorHeap
{
public:
	/// <summary>
	/// ディスクリプタヒープの初期化
	/// </summary>
	/// <param name="type">ヒープタイプ（例えば、CBV、SRV、UAVなど）</param>
	/// <param name="numDescriptors">ヒープ内のディスクリプタ数</param>
	/// <param name="shaderVisible">シェーダーからアクセス可能かどうか</param>
	void Initialize(D3D12_DESCRIPTOR_HEAP_TYPE type, UINT numDescriptors, bool shaderVisible);

	/// <summary>
	/// ディスクリプタヒープから次の使用可能なディスクリプタを割り当
	/// </summary>
	/// <returns>割り当てられたディスクリプタのハンドル</returns>
	DescriptorHandle Allocate();

	/// <summary>
	/// ディスクリプタのサイズを取得
	/// </summary>
	/// <returns>ディスクリプタのサイズ（バイト単位）</returns>
	uint32_t GetDescriptorSize() const { return descriptorSize_; }

	/// <summary>
	/// ディスクリプタヒープのポインタを取得
	/// </summary>
	/// <returns>ID3D12DescriptorHeapインターフェースへのポインタ</returns>
	ID3D12DescriptorHeap* GetDescriptorHeap() const { return descriptorHeap_.Get(); };

private:
	/// <summary>
	/// ディスクリプタヒープのCOMポインタ
	/// </summary>
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap_ = nullptr;

	/// <summary>
	/// ディスクリプタのサイズ（バイト単位）
	/// </summary>
	UINT descriptorSize_ = 0;

	/// <summary>
	/// 使用可能なディスクリプタの数
	/// </summary>
	uint32_t numFreeDescriptors_ = 0;

	/// <summary>
	/// 最初に割り当てるディスクリプタのハンドル
	/// </summary>
	DescriptorHandle firstHandle_;

	/// <summary>
	/// 次に使用可能なディスクリプタのハンドル
	/// </summary>
	DescriptorHandle nextFreeHandle_;
};
