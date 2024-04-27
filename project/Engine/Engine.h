#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

class Engine {
public:
	static Engine* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="title">ウィンドウのタイトル</param>
	/// <param name="width">横幅</param>
	/// <param name="height">縦幅</param>
	static void Initialize(const wchar_t* title, int width, int height);

	/// <summary>
	/// 後始末
	/// </summary>
	static void Finalize();

	static bool ProcessMessage();

	static void BeginFrame();

	static void EndFrame();

	static void PostDraw();

	static void PreDraw();

	static void RenderPreDraw();
	static void RenderPostDraw();

	static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
	    ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);
	static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
	    ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);

public:
	static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice();
	static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetList();
	
	// SRV用のヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRV();
	 // RTV用のヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTV();
	 // DSV用のヒープ
	static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDSV();

	const DescriptorHandle& GetHandle();

	const D3D12_GPU_DESCRIPTOR_HANDLE GetSRVHandle();

};
