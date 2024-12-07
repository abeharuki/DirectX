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
	//頂点数と要素数
	void Create(uint32_t numElements, uint32_t elementSize);

	void* Map();

	void Unmap();

	const DescriptorHandle& GetSRVHandle() { return srvHandle_; };

	size_t GetBufferSize() const { return bufferSize_; };

private:
	void CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize);

private:
	DescriptorHandle srvHandle_{};

	size_t bufferSize_ = 0;

	uint32_t elementCount_ = 0;

	uint32_t elementSize_ = 0;
};

