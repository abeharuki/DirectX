#pragma once
#include "StringUtility.h"
#include "WinApp.h"
#include "ResourceObject.h"
#include <Windows.h>

#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>

#include <cstdint>
#include <wrl.h>
#include <vector>
#include <dxgidebug.h>

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "dxguid.lib")

/// <summary>
/// DirectX汎用
/// </summary>
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
	Microsoft::WRL::ComPtr<ID3D12Resource> depthBuffer_;
	// SRV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap_;
	// RTV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvHeap_;
	// DSV用のヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvHeap_;
	Microsoft::WRL::ComPtr<ID3D12Fence> fence_;
	Microsoft::WRL::ComPtr <IDXGIDebug1> debug_;

	uint64_t fenceVal_ = 0;
	int32_t backBufferWidth_ = 0;
	int32_t backBufferHeight_ = 0;
	HANDLE frameLatencyWaitableObject_;
	int32_t refreshRate_ = 0;
	Utility* utility_;
	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles_[2];

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
};