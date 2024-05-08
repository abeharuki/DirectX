#pragma once
#include "WinApp.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <array>
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>
#include <DescriptorHeap.h>


#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")


class SwapChain {
public:
	void Initialize(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);

	void Present();

	UINT GetCurrentBackBuffer();

	ID3D12Resource* GetResource(int num) { return swapChainResources[num].Get(); };
	ID3D12Resource* GetResource() const { 
		UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();
		return swapChainResources[backBufferIndex].Get(); 
	};

	//size_t GetBackBufferCount() const { return swapChainResources.size; }
private:
	void CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue);
	void CreateResource();
private:
	
	WinApp* winApp_ = nullptr;

	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	Microsoft::WRL::ComPtr<ID3D12Resource> swapChainResources[2];
};