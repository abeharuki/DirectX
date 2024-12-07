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
	
	//頂点数と要素数
	void Create(uint32_t numElements, uint32_t elementSize);

	const DescriptorHandle& GetSRVHandle() const { return srvHandle_; };

	const DescriptorHandle& GetUAVHandle() { return uavHandle_; };

	size_t GetBufferSize() const { return bufferSize_; };

private:
	void CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize);

private:
	DescriptorHandle srvHandle_{};
	DescriptorHandle uavHandle_{};

	size_t bufferSize_ = 0;
};

