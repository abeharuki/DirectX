#include "SwapChain.h"
#include <cassert>


void SwapChain::Initialize(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue) {
	winApp_ = WinApp::GetInstance();

	CreateSwapChain(dxgiFactory, commandQueue);
	CreateResource();
}

void SwapChain::CreateSwapChain(IDXGIFactory7* dxgiFactory, ID3D12CommandQueue* commandQueue) {

	HRESULT hr_ = S_FALSE;

	// スワップチェーンを生成する

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = winApp_->kWindowWidth;                      // 画面の幅
	swapChainDesc.Height = winApp_->kWindowHeight;                    // 画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // 色の形式
	swapChainDesc.SampleDesc.Count = 1;                          // マルチサンプル
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用
	swapChainDesc.BufferCount = 2;                               /// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄

	// コマンドキュー、ウインドウハンドル、設定を渡して生成する
	hr_ = dxgiFactory->CreateSwapChainForHwnd(
		commandQueue, winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr_));
}

void SwapChain::Present()
{
	//GPUとOSに画面の交換を行うよう通知する
	swapChain_->Present(1, 0);
}

void SwapChain::CreateResource(){
	HRESULT hr_ = S_FALSE;

	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr_));
	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr_));
}



UINT SwapChain::GetCurrentBackBuffer() {
	return swapChain_->GetCurrentBackBufferIndex();
}