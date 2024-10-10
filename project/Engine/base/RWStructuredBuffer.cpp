#include "RWStructuredBuffer.h"
#include <cassert>
#include <DirectXCommon.h>

void RWStructuredBuffer::Create(uint32_t numElements, uint32_t elementSize){
	ID3D12Device* device = DirectXCommon::GetInstance()->GetDevice();


	bufferSize_ = numElements * elementSize;

	currentState_ = D3D12_RESOURCE_STATE_COMMON;

	//リソース用のヒープの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT;

	//リソースの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
	resourceDesc.Width = bufferSize_;
	resourceDesc.Height = 1;
	resourceDesc.DepthOrArraySize = 1;
	resourceDesc.MipLevels = 1;
	resourceDesc.SampleDesc.Count = 1;
	resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_UNORDERED_ACCESS;

	//作成
	HRESULT hr = device->CreateCommittedResource(&heapProperties, D3D12_HEAP_FLAG_NONE,
		&resourceDesc, currentState_, nullptr,
		IID_PPV_ARGS(&resource_));
	assert(SUCCEEDED(hr));
	hr;
	//GpuVirtualAddressの初期化
	gpuVirtualAddress_ = resource_->GetGPUVirtualAddress();

	//Viewの作成
	CreateDerivedViews(device, numElements, elementSize);

}

void RWStructuredBuffer::CreateDerivedViews(ID3D12Device* device, uint32_t numElements, uint32_t elementSize){
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	srvDesc.Buffer.FirstElement = 0;
	srvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	srvDesc.Buffer.NumElements = numElements;//頂点数
	srvDesc.Buffer.StructureByteStride = UINT(elementSize);
	srvHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	device->CreateShaderResourceView(resource_.Get(), &srvDesc, srvHandle_);


	//srv作成
	D3D12_UNORDERED_ACCESS_VIEW_DESC uavDesc{};
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.ViewDimension = D3D12_UAV_DIMENSION_BUFFER;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = numElements;//頂点数
	uavDesc.Buffer.CounterOffsetInBytes = 0;
	uavDesc.Buffer.Flags = D3D12_BUFFER_UAV_FLAG_NONE;
	uavDesc.Buffer.StructureByteStride = elementSize;
	uavHandle_ = DirectXCommon::GetInstance()->AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);

	// UAVビューを作成
	device->CreateUnorderedAccessView(resource_.Get(), nullptr, &uavDesc, uavHandle_);
}


