#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

class Engine {
public:
	static Engine* GetInstance();

	static void Initialize(const char* title, int width, int height);
	
	static void Finalize();

	static bool BeginFrame();

	static void EndFrame();

public:
	static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice();
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetList();
	
	// SRV用のヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRV();
	 // RTV用のヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTV();
	 // DSV用のヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDSV();

	
};
