#pragma once
#include "StringUtility.h"
#include "WinApp.h"
#include <Windows.h>

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
#include <DescriptorHandle.h>
#include "Math/math.h"

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

/**
 * @class DirectXCommon
 * @brief DirectX 12 に関する基本的な初期化、レンダリング、リソース管理などを行うクラス
 */
class DirectXCommon {
public: // メンバ関数
	/// <summary>
	/// シングルトンインスタンスの取得
	/// </summary>
	/// <returns></returns>
	static DirectXCommon* GetInstance();

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight);

	/// <summary>
	/// 後始末
	/// </summary>
	void Finalize();

	/// <summary>
	/// 描画前処理
	/// </summary>
	void PreDraw();

	/// <summary>
	/// 描画後処理
	/// </summary>
	void PostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTarget();

	/// <summary>
	/// 深度バッファのクリア
	/// </summary>
	void ClearDepthBuffer();


	//リソースリークチェック
	void Debug();

	//レンダー関連
	/// <summary>
	/// 描画前処理
	/// </summary>
	void RenderPreDraw();
	void RenderPostDraw();

	//深度
	/// <summary>
	/// 描画前処理
	/// </summary>
	void DepthPreDraw();
	void DepthPostDraw();

	/// <summary>
	/// レンダーターゲットのクリア
	/// </summary>
	void ClearRenderTargetSWAP();


	/// <summary>
	/// レンダーテクスチャの作成
	/// </summary>
	void CreateRenderTexture();
	Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format,const Vector4& clearColor);

	//アロケーター
	DescriptorHandle AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type);
	//ディスクリプタヒープゲッター
	ID3D12DescriptorHeap* GetDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type) const { return descriptorHeaps_[type]->GetDescriptorHeap(); }

	/// <summary>
	/// デバイスの取得
	/// </summary>
	/// <returns>デバイス</returns>
	ID3D12Device* GetDevice() const { return device_.Get(); }

	/// <summary>
	/// 描画コマンドリストの取得
	/// </summary>
	/// <returns>描画コマンドリスト</returns>
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList_.Get(); }

	/// <summary>
	/// バックバッファの幅取得
	/// </summary>
	/// <returns>バックバッファの幅</returns>
	int32_t GetBackBufferWidth() const;

	/// <summary>
	/// バックバッファの高さ取得
	/// </summary>
	/// <returns>バックバッファの高さ</returns>
	int32_t GetBackBufferHeight() const;

	// バックバッファの数を取得
	size_t GetBackBufferCount() const { return swapChainResources.size(); }

	//各ヒープのゲッター
	ID3D12DescriptorHeap* GetSRV() const { return  descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetDescriptorHeap(); }
	ID3D12DescriptorHeap* GetRTV() const { return descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_RTV]->GetDescriptorHeap(); }
	ID3D12DescriptorHeap* GetDSV() const { return descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GetDescriptorHeap(); }

	const DescriptorHandle& GetHandle() { return srvHandle_; }
	const DescriptorHandle& GetDepthHandle() { return depthHandle_; }

	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVHandle() { return dsvHandle_; }

	static const uint32_t kMaxSRVCount;

private: // メンバ変数
	// ウィンドウズアプリケーション管理
	WinApp* winApp_;

	// Direct3D関連
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory_;
	Microsoft::WRL::ComPtr<ID3D12Device> device_;
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList_;
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator_;
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue_;
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain_;
	std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> swapChainResources;
	Microsoft::WRL::ComPtr<ID3D12Resource> depthStencilResource_;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	Microsoft::WRL::ComPtr <IDXGIDebug1> debug_;

	uint64_t fenceVal_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	HANDLE frameLatencyWaitableObject_;
	int32_t refreshRate_ = 0;
	D3D12_VIEWPORT viewport;
	D3D12_RECT scissorRect;

	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle_;
	DescriptorHandle srvHandle_;
	D3D12_RESOURCE_BARRIER barrier{};
	DescriptorHandle depthHandle_;
	D3D12_RESOURCE_BARRIER depthBarrier{};
	DescriptorHandle dsvHandle_;

	Microsoft::WRL::ComPtr<ID3D12Resource> renderTextureResource;
	D3D12_RESOURCE_BARRIER renderBarrier{};

	std::chrono::steady_clock::time_point reference_;

	std::array<std::unique_ptr<DescriptorHeap>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> descriptorHeaps_{};
	std::array<const uint32_t, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES> kNumDescriptors_ = { 512, 256,256, 256, };
private: // メンバ関数
	DirectXCommon() = default;
	~DirectXCommon() = default;
	DirectXCommon(const DirectXCommon&) = delete;
	const DirectXCommon& operator=(const DirectXCommon&) = delete;

	/// <summary>
	/// DXGIデバイス初期化
	/// </summary>
	void InitializeDXGIDevice();

	/// <summary>
	/// スワップチェーンの生成
	/// </summary>
	void CreateSwapChain();

	/// <summary>
	/// コマンド関連初期化
	/// </summary>
	void InitializeCommand();

	/// <summary>
	/// レンダーターゲット生成
	/// </summary>
	void CreateFinalRenderTargets();

	/// <summary>
	/// 深度バッファ生成
	/// </summary>
	void CreateDepthBuffer();

	/// <summary>
	/// フェンス生成
	/// </summary>
	void CreateFence();

	void InitializeFixFPS();
	void UpdateFixFPS();

	

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
		D3D12_CPU_DESCRIPTOR_HANDLE handleCPU = descriptorHeap->GetCPUDescriptorHandleForHeapStart();
		handleCPU.ptr += (descriptorSize * index);
		return handleCPU;
	}

	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
		ID3D12DescriptorHeap* descriptorHeap, uint32_t descriptorSize, uint32_t index) {
		D3D12_GPU_DESCRIPTOR_HANDLE handleGPU = descriptorHeap->GetGPUDescriptorHandleForHeapStart();
		handleGPU.ptr += (descriptorSize * index);
		return handleGPU;
	}

};

ID3D12DescriptorHeap* CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
	bool shaderVisible);