#include "DirectXCommon.h"
#include <format>
#include <DirectXTex.h>
#include <d3dx12.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
#include <thread>
#include <Math/math.h>



using namespace Microsoft::WRL;
using namespace Microsoft::WRL;
const uint32_t DirectXCommon::kMaxSRVCount = 512;

ID3D12DescriptorHeap* CreateDescriptorHeap(
	ID3D12Device* device, D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDescriptors,
	bool shaderVisible) {
	// ディスクリプタヒープの生成
	ID3D12DescriptorHeap* descriptorHeap = nullptr;
	D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc{};
	descriptorHeapDesc.Type = heapType;                 // レンダーターゲットビュー用
	descriptorHeapDesc.NumDescriptors = numDescriptors; // ダブルバッファ用に二つ。多くても可
	descriptorHeapDesc.Flags =
		shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HRESULT hr = device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(&descriptorHeap));
	// ディスクリプタヒープが作れなかったので起動できない
	assert(SUCCEEDED(hr));

	return descriptorHeap;
};


DirectXCommon* DirectXCommon::GetInstance() {
	static DirectXCommon instance;
	return &instance;
}

void DirectXCommon::Finalize() {
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

}


void DirectXCommon::Initialize(WinApp* winApp, int32_t backBufferWidth, int32_t backBufferHeight) {
	// nullptrチェック
	assert(winApp);
	assert(4 <= backBufferWidth && backBufferWidth <= 4096);
	assert(4 <= backBufferHeight && backBufferHeight <= 4096);

	// sleepの分解能をあげておく
	//timeBeginPeriod(1);

	winApp_ = winApp;

	//FPSの固定
	InitializeFixFPS();

	// DXGIデバイス初期化
	InitializeDXGIDevice();

	// コマンド関連初期化
	InitializeCommand();


	for (uint32_t i = 0; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; ++i)
	{
		descriptorHeaps_[i] = std::make_unique<DescriptorHeap>();
		bool shaderVisible = (i == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) ? true : false;
		descriptorHeaps_[i]->Initialize(D3D12_DESCRIPTOR_HEAP_TYPE(i), kNumDescriptors_[i], shaderVisible);
	}

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	//レンダーテクスチャの作成
	CreateRenderTexture();

	// 深度バッファ生成
	CreateDepthBuffer();

	// フェンス生成
	CreateFence();
}

void DirectXCommon::PreDraw() {


	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex].Get();
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを練る
	commandList_->ResourceBarrier(1, &barrier);

	// 描画先のRTVとDSVを設定する 
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();


	commandList_->OMSetRenderTargets(1, &rtvHandles_[backBufferIndex], false, &dsvHandle_);

	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kWindowWidth;
	viewport.Height = WinApp::kWindowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形

	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kWindowHeight;

	// 全画面クリア
	ClearRenderTargetSWAP();


	//  コマンドを積む
	commandList_->RSSetViewports(1, &viewport);
	commandList_->RSSetScissorRects(1, &scissorRect);

	ID3D12DescriptorHeap* heaps[] = { descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetDescriptorHeap() };
	commandList_->SetDescriptorHeaps(_countof(heaps), heaps);

}

void DirectXCommon::PostDraw() {
	HRESULT hr_ = S_FALSE;

	// 今回はRenderTargetからPresentにする
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	// TransitionBarrierを練る
	commandList_->ResourceBarrier(1, &barrier);
	// コマンドリストの内容を確定させる。すべてのコマンドを積んでからcloseすること
	hr_ = commandList_->Close();
	assert(SUCCEEDED(hr_));

	// GPUにコマンドの実行を行わせる
	ID3D12CommandList* commandLists[] = { commandList_.Get() };
	commandQueue_->ExecuteCommandLists(1, commandLists);
	// GPUとOSに画像の交換を行うよう通知する
	hr_ = swapChain_->Present(1, 0);
	assert(SUCCEEDED(hr_));
	// Fenceの値を更新
	// GPUがここまでたどり着いたときに、Fenceの値を指定した値に代入するようにsignalを送る
	commandQueue_->Signal(fence_.Get(), ++fenceVal_);
	// Fenceの値が指定したSignal値にたどりついているか確認する
	// GetCompletedvalueの初期値はFence作成時に渡した初期値
	if (fence_->GetCompletedValue() < fenceVal_) {
		HANDLE fenceEvent_ = CreateEvent(nullptr, false, false, nullptr);
		// 指定したsignalにたどり着いていないので、たどり着くまでイベントを設定する
		fence_->SetEventOnCompletion(fenceVal_, fenceEvent_);
		// イベントをまつ
		WaitForSingleObject(fenceEvent_, INFINITE);
		CloseHandle(fenceEvent_);
	}

	//FPS固定
	UpdateFixFPS();

	// 次のフレーム用のコマンドリストを準備
	hr_ = commandAllocator_->Reset();
	assert(SUCCEEDED(hr_));
	hr_ = commandList_->Reset(commandAllocator_.Get(), nullptr);
	assert(SUCCEEDED(hr_));
}

//指定した色で画面全体をクリア
void DirectXCommon::ClearRenderTargetSWAP() {
	HRESULT hr_ = S_FALSE;
	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();

	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList_->ClearRenderTargetView(rtvHandles_[backBufferIndex], clearColor, 0, nullptr);

	assert(SUCCEEDED(hr_));

}

void DirectXCommon::ClearDepthBuffer() {
	// 深度ステンシルビュー用デスクリプタヒープのハンドルを取得
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsvH =
		CD3DX12_CPU_DESCRIPTOR_HANDLE(descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart());
	// 深度バッファのクリア
	commandList_->ClearDepthStencilView(dsvHandle_, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}




int32_t DirectXCommon::GetBackBufferWidth() const { return backBufferWidth_; }

int32_t DirectXCommon::GetBackBufferHeight() const { return backBufferHeight_; }

//DXGIファクトリー
void DirectXCommon::InitializeDXGIDevice() {
	HRESULT hr_ = S_FALSE;

#ifdef _DEBUG
	ComPtr<ID3D12Debug1> debugController;

	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーをオンに
		debugController->EnableDebugLayer();
		// GPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}

#endif

	// DXGIファクトリーの生成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	// ダイレクトXとデバイスをつなぐアダプタ
	// 使用するアダプタ用の変数
	ComPtr< IDXGIAdapter4 > useAdapter;
	// いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(
		i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
		DXGI_ERROR_NOT_FOUND; ++i) {
		// アダプタの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。
			Utility::Log(Utility::ConvertString((L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかったら起動できない
	assert(useAdapter != nullptr);

	D3D_FEATURE_LEVEL featureLevels[] = {
		D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0 };
	const char* featureLevelStrings[] = { "12.2", "12.2", "12.0" };
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を使ってループを抜ける
			Utility::Log(("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	Utility::Log("Complete create D3D12Device!!!\n");

#ifdef _DEBUG
	ID3D12InfoQueue* infoQueue = nullptr;
	if (SUCCEEDED(device_->QueryInterface(IID_PPV_ARGS(&infoQueue)))) {
		// やばいエラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
		// エラー時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
		// 警告時に止まる
		infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

		// 抑制するメッセージのID
		D3D12_MESSAGE_ID denyIds[] = {
			// Window11でのDXGIデバッグレイヤーとDX12デバッグレイヤーの相互作用バグによるエラーメッセージ
			// https://stackflow.com/questions/69805245/directx-12-application-is-crashing-in-windows-11
			D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE };
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = { D3D12_MESSAGE_SEVERITY_INFO };
		D3D12_INFO_QUEUE_FILTER filter{};
		filter.DenyList.NumIDs = _countof(denyIds);
		filter.DenyList.pIDList = denyIds;
		filter.DenyList.NumSeverities = _countof(severities);
		filter.DenyList.pSeverityList = severities;
		// 指定したメッセージの表示を抑制する
		infoQueue->PushStorageFilter(&filter);

		// 解放
		infoQueue->Release();
	}
#endif


}

//スワップチェーン
void DirectXCommon::CreateSwapChain() {

	HRESULT hr_ = S_FALSE;

	// スワップチェーンを生成する

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = backBufferWidth_;                      // 画面の幅
	swapChainDesc.Height = backBufferHeight_;                    // 画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // 色の形式
	swapChainDesc.SampleDesc.Count = 1;                          // マルチサンプル
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用
	swapChainDesc.BufferCount = 2;                               /// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄

	// コマンドキュー、ウインドウハンドル、設定を渡して生成する
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(
		commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr,
		reinterpret_cast<IDXGISwapChain1**>(swapChain_.GetAddressOf()));
	assert(SUCCEEDED(hr_));

}

//コマンド
void DirectXCommon::InitializeCommand() {
	HRESULT hr_ = S_FALSE;

	// コマンドキューを生成する

	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue_));

	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));

	// コマンドアロケータを生成する

	hr_ = device_->CreateCommandAllocator(
		D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator_));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));

	// コマンドリストを生成する

	hr_ = device_->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator_.Get(), nullptr,
		IID_PPV_ARGS(&commandList_));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

//ディスクリプターヒープ
void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT hr_ = S_FALSE;



	swapChainResources.resize(2);
	// SwapChainからResourceを引っ張ってくる
	hr_ = swapChain_->GetBuffer(0, IID_PPV_ARGS(&swapChainResources[0]));
	// うまく取得できなければ起動できない
	assert(SUCCEEDED(hr_));
	hr_ = swapChain_->GetBuffer(1, IID_PPV_ARGS(&swapChainResources[1]));
	assert(SUCCEEDED(hr_));



	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	// ディスクリプタの先頭を取得する
	//D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle =rtvHeap_->GetCPUDescriptorHandleForHeapStart();

	// まず1つ目を作る。１つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles_[0] = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device_->CreateRenderTargetView(swapChainResources[0].Get(), &rtvDesc, rtvHandles_[0]);
	// 2つ目のディスクリプタハンドルを得る(自力で)
	rtvHandles_[1] = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResources[1].Get(), &rtvDesc, rtvHandles_[1]);


}

// 深度情報を持ったTexture
ID3D12Resource*
CreateDepthStencilTextureResource(ID3D12Device* device, int32_t width, int32_t height) {
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                          // Textureの幅
	resourceDesc.Height = height;                        // Textureの高さ
	resourceDesc.MipLevels = 1;                          // mipmapの数
	resourceDesc.DepthOrArraySize = 1;                   // 奥行き or 配列Textureの配列
	resourceDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL; // DepthStencilとして使う通知

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE depthClearValue{};
	depthClearValue.DepthStencil.Depth = 1.0f;              // 1.0f(最大値)クリア
	depthClearValue.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // フォーマット。Resourceと合わせる

	// Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,                  // Heapの設定
		D3D12_HEAP_FLAG_NONE,             // Heapの特殊な設定。特になし。
		&resourceDesc,                    // Resourceの設定
		D3D12_RESOURCE_STATE_DEPTH_WRITE, // 深度値を書き込む状態にしておく
		&depthClearValue,                 // Clear最適値
		IID_PPV_ARGS(&resource));         // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}

// 深度バッファ
void DirectXCommon::CreateDepthBuffer() {


	ID3D12Resource* depthStencilResource = CreateDepthStencilTextureResource(
		device_.Get(), WinApp::kWindowWidth, WinApp::kWindowHeight);


	// DSVの設定
	D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc{};
	dsvDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; // Format。基本的にはResourceに合わせる
	dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D; // 2dTexture

	dsvHandle_ = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	// DSVHeapの先頭にDSVを作る
	device_->CreateDepthStencilView(depthStencilResource, &dsvDesc, dsvHandle_);


}

//Fence
void DirectXCommon::CreateFence() {
	HRESULT hr_ = S_FALSE;


	hr_ = device_->CreateFence(fenceVal_, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence_));
	assert(SUCCEEDED(hr_));


}

void ::DirectXCommon::InitializeFixFPS() { reference_ = std::chrono::steady_clock::now(); }

void ::DirectXCommon::UpdateFixFPS() {
	// 1/60秒ぴったりの時間
	const std::chrono::microseconds kMinTime(uint64_t(1000000.0f / 60.0f));

	// 1/60秒よりわずかに短い時間
	const std::chrono::microseconds kMinCheckTime(uint64_t(1000000.0f / 65.0f));

	// 現在時間の取得
	std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
	// 前回記録からの経過時間を取得するn
	std::chrono::microseconds elapsed =
		std::chrono::duration_cast<std::chrono::microseconds>(now - reference_);

	// 1/60秒(よりわずかに短い時間)経っていない場合
	if (elapsed < kMinTime) {
		// 1/60秒経過するまでスリープを繰り返す
		while (std::chrono::steady_clock::now() - reference_ < kMinTime) {
			// 1マイクロ秒スリープ
			std::this_thread::sleep_for(std::chrono::microseconds(1));
		}
	}

	// 現在の時間を取得する
	reference_ = std::chrono::steady_clock::now();
}

//リソースチェック
void DirectXCommon::Debug() {


	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&debug_)))) {
		debug_->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_ALL);
		debug_->ReportLiveObjects(DXGI_DEBUG_APP, DXGI_DEBUG_RLO_ALL);
		debug_->ReportLiveObjects(DXGI_DEBUG_D3D12, DXGI_DEBUG_RLO_ALL);
	}
}


/*-------------------------OffscreenRendering関連------------------------*/

Microsoft::WRL::ComPtr<ID3D12Resource> CreateRenderTextureResource(Microsoft::WRL::ComPtr<ID3D12Device> device, uint32_t width, uint32_t height, DXGI_FORMAT format, const Vector4& clearColor) {
	// 生成するResourceの設定
	D3D12_RESOURCE_DESC resourceDesc{};
	resourceDesc.Width = width;                          // Textureの幅
	resourceDesc.Height = height;                       // Textureの高さ
	resourceDesc.MipLevels = 1;                          // mipmapの数
	resourceDesc.DepthOrArraySize = 1;                   // 奥行き or 配列Textureの配列
	resourceDesc.Format = format; // DepthStencilとして利用可能なフォーマット
	resourceDesc.SampleDesc.Count = 1; // サンプリングカウント。1固定。
	resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D; // 2次元
	resourceDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET; // RenderTargetとして利用可能にする

	// 利用するHeapの設定
	D3D12_HEAP_PROPERTIES heapProperties{};
	heapProperties.Type = D3D12_HEAP_TYPE_DEFAULT; // VRAM上に作る

	// 深度値のクリア設定
	D3D12_CLEAR_VALUE clearValue{};
	clearValue.Format = format; // フォーマット。Resourceと合わせる
	clearValue.Color[0] = clearColor.x;
	clearValue.Color[1] = clearColor.y;
	clearValue.Color[2] = clearColor.z;
	clearValue.Color[3] = clearColor.w;


	// Resourceを生成する
	ID3D12Resource* resource = nullptr;
	HRESULT hr = device->CreateCommittedResource(
		&heapProperties,                  // Heapの設定
		D3D12_HEAP_FLAG_NONE,             // Heapの特殊な設定。特になし。
		&resourceDesc,                    // Resourceの設定
		D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE, // 深度値を書き込む状態にしておく
		&clearValue,                 // Clear最適値
		IID_PPV_ARGS(&resource));         // 作成するResourceポインタへのポインタ
	assert(SUCCEEDED(hr));
	return resource;
}


//指定した色で画面全体をクリア
void DirectXCommon::ClearRenderTarget() {
	HRESULT hr_ = S_FALSE;


	// 指定した色で画面全体をクリアする
	float clearColor[] = { 0.1f, 0.25f, 0.5f, 1.0f };
	commandList_->ClearRenderTargetView(rtvHandle_, clearColor, 0, nullptr);
	assert(SUCCEEDED(hr_));

}
void DirectXCommon::RenderPreDraw() {



	// 今回のバリアはTransition
	renderBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	//Noneにしておく
	renderBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	//バリアを張る対象のリソース。現在のバックバッファに対して行う
	renderBarrier.Transition.pResource = renderTextureResource.Get();
	//遷移前(現在)のResourceState
	renderBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	//遷移後のResourceState
	renderBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	//TransitionBarrierを練る
	commandList_->ResourceBarrier(1, &renderBarrier);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_DSV]->GetDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();


	commandList_->OMSetRenderTargets(1, &rtvHandle_, false, &dsvHandle_);


	// クライアント領域のサイズと一緒にして画面全体に表示
	viewport.Width = WinApp::kWindowWidth;
	viewport.Height = WinApp::kWindowHeight;
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// シザー矩形

	// 基本的にビューポートと同じ矩形が構成されるようにする
	scissorRect.left = 0;
	scissorRect.right = WinApp::kWindowWidth;
	scissorRect.top = 0;
	scissorRect.bottom = WinApp::kWindowHeight;

	// 全画面クリア
	ClearRenderTarget();
	// 深度バッファクリア
	ClearDepthBuffer();
	//  コマンドを積む
	commandList_->RSSetViewports(1, &viewport);
	commandList_->RSSetScissorRects(1, &scissorRect);

	ID3D12DescriptorHeap* heaps[] = { descriptorHeaps_[D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV]->GetDescriptorHeap() };
	commandList_->SetDescriptorHeaps(_countof(heaps), heaps);

}

void DirectXCommon::RenderPostDraw() {
	HRESULT hr_ = S_FALSE;

	// 今回はRenderTargetからPresentにする
	renderBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	renderBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE;
	// TransitionBarrierを練る
	commandList_->ResourceBarrier(1, &renderBarrier);

}


//レンダーテクスチャ
void DirectXCommon::CreateRenderTexture() {

	//RTVの作成
	const Vector4 kRenderTargetClearValue{ 0.1f, 0.25f, 0.5f, 1.0f };;
	renderTextureResource = CreateRenderTextureResource(device_.Get(), WinApp::kWindowWidth, WinApp::kWindowHeight, DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, kRenderTargetClearValue);

	// RTVの設定
	D3D12_RENDER_TARGET_VIEW_DESC rtvDesc{};
	rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	rtvDesc.ViewDimension = D3D12_RTV_DIMENSION_TEXTURE2D;

	rtvHandle_ = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	device_->CreateRenderTargetView(renderTextureResource.Get(), &rtvDesc, rtvHandle_);

	//SRVの設定
	D3D12_SHADER_RESOURCE_VIEW_DESC renderTextureSrvDesc{};
	renderTextureSrvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	renderTextureSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	renderTextureSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
	renderTextureSrvDesc.Texture2D.MipLevels = 1;

	srvHandle_ = AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	//SRVの作成
	device_->CreateShaderResourceView(renderTextureResource.Get(), &renderTextureSrvDesc, srvHandle_);

}



DescriptorHandle DirectXCommon::AllocateDescriptor(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	return descriptorHeaps_[type]->Allocate();
}

