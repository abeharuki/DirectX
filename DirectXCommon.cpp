#include "DirectXCommon.h"
#include "StringUtility.h"
#include <format>


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


void DirectXCommon::Initialize(WinApp* winApp) {
	// nullptrチェック
	assert(winApp);
	
	// sleepの分解能をあげておく
	timeBeginPeriod(1);

	winApp_ = winApp;
	reference_ = std::chrono::steady_clock::now();
	rtvHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_RTV, 2, false);
	srvHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, 128, true);
	dsvHeap_ = CreateDescriptorHeap(device_.Get(), D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1, false);

	// DXGIデバイス初期化
	InitializeDXGIDevice();

	// コマンド関連初期化
	InitializeCommand();

	// スワップチェーンの生成
	CreateSwapChain();

	// レンダーターゲット生成
	CreateFinalRenderTargets();

	// 深度バッファ生成
	CreateDepthBuffer();

	// フェンス生成
	CreateFence();
}



void DirectXCommon::ClearRenderTarget() {
	
}

//DXGIファクトリー
void DirectXCommon::InitializeDXGIDevice() { 
	HRESULT hr_ = S_FALSE;



	// DXGIファクトリーの生成
	HRESULT hr = CreateDXGIFactory(IID_PPV_ARGS(&dxgiFactory_));
	assert(SUCCEEDED(hr));

	// ダイレクトXとデバイスをつなぐアダプタ
	// 使用するアダプタ用の変数
	Microsoft::WRL::ComPtr< IDXGIAdapter4 > useAdapter;
	// いい順にアダプタを頼む
	for (UINT i = 0; dxgiFactory_->EnumAdapterByGpuPreference(
	                     i, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&useAdapter)) !=
	                 DXGI_ERROR_NOT_FOUND;
	     ++i) {
		// アダプタの情報を取得
		DXGI_ADAPTER_DESC3 adapterDesc{};
		hr_ = useAdapter->GetDesc3(&adapterDesc);
		assert(SUCCEEDED(hr));
		// ソフトウェアアダプタでなければ採用
		if (!(adapterDesc.Flags & DXGI_ADAPTER_FLAG3_SOFTWARE)) {
			// 採用したアダプタの情報をログに出力。
			utility_->Log(utility_->ConvertString(std::format(L"Use Adapater:{}\n", adapterDesc.Description)));
			break;
		}
		useAdapter = nullptr;
	}
	// 適切なアダプタが見つからなかったら起動できない
	assert(useAdapter != nullptr);

	D3D_FEATURE_LEVEL featureLevels[] = {
	    D3D_FEATURE_LEVEL_12_2, D3D_FEATURE_LEVEL_12_1, D3D_FEATURE_LEVEL_12_0};
	const char* featureLevelStrings[] = {"12.2", "12.2", "12.0"};
	// 高い順に生成できるか試していく
	for (size_t i = 0; i < _countof(featureLevels); ++i) {
		// 採用したアダプタでデバイスを生成
		hr = D3D12CreateDevice(useAdapter.Get(), featureLevels[i], IID_PPV_ARGS(&device_));
		// 指定した機能レベルでデバイスが生成できたか確認
		if (SUCCEEDED(hr)) {
			// 生成できたのでログ出力を使ってループを抜ける
			utility_->Log(std::format("FeatureLevel : {}\n", featureLevelStrings[i]));
			break;
		}
	}
	// デバイスの生成がうまくいかなかったので起動できない
	assert(device_ != nullptr);
	utility_->Log("Complete create D3D12Device!!!\n");

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
		    D3D12_MESSAGE_ID_RESOURCE_BARRIER_MISMATCHING_COMMAND_LIST_TYPE};
		// 抑制するレベル
		D3D12_MESSAGE_SEVERITY severities[] = {D3D12_MESSAGE_SEVERITY_INFO};
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
	IDXGISwapChain4* swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	swapChainDesc.Width = winApp_->kWindowWidth;                          // 画面の幅
	swapChainDesc.Height = winApp_->kWindowHeight;                        // 画面の高さ
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;           // 色の形式
	swapChainDesc.SampleDesc.Count = 1;                          // マルチサンプル
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // 描画のターゲットとして利用
	swapChainDesc.BufferCount = 2;                               /// ダブルバッファ
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // モニタに写したら中身を破棄
	// コマンドキュー、ウインドウハンドル、設定を渡して生成する
	hr_ = dxgiFactory_->CreateSwapChainForHwnd(
	    commandQueue_.Get(), winApp_->GetHwnd(), &swapChainDesc, nullptr, nullptr,
	    reinterpret_cast<IDXGISwapChain1**>(&swapChain));
	assert(SUCCEEDED(hr_));

}

//コマンド
void DirectXCommon::InitializeCommand() {
	HRESULT hr_ = S_FALSE;

	// コマンドキューを生成する
	ID3D12CommandQueue* commandQueue = nullptr;
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc{};
	hr_ = device_->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue));

	// コマンドキューの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));

	// コマンドアロケータを生成する
	ID3D12CommandAllocator* commandAllocator = nullptr;
	hr_ = device_->CreateCommandAllocator(
	    D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator));
	// コマンドアロケータの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));

	// コマンドリストを生成する
	ID3D12GraphicsCommandList* commandList = nullptr;
	hr_ = device_->CreateCommandList(
	    0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList));
	// コマンドリストの生成がうまくいかなかったので起動できない
	assert(SUCCEEDED(hr_));
}

//ディスクリプターヒープRTV
void DirectXCommon::CreateFinalRenderTargets() {
	HRESULT hr_ = S_FALSE;

	// SwapChainからResourceを引っ張ってくる
	ID3D12Resource* swapChainResources[2] = {nullptr};
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
	D3D12_CPU_DESCRIPTOR_HANDLE rtvStartHandle =
	    rtvHeap_->GetCPUDescriptorHandleForHeapStart();
	// RTVを2つ作るのでディスクリプタを2つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];
	// まず1つ目を作る。１つ目は最初のところに作る。作る場所をこちらで指定してあげる必要がある
	rtvHandles[0] = rtvStartHandle;
	device_->CreateRenderTargetView(swapChainResources[0], &rtvDesc, rtvHandles[0]);
	// 2つ目のディスクリプタハンドルを得る(自力で)
	rtvHandles[1].ptr = rtvHandles[0].ptr +
	                    device_->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	// 2つ目を作る
	device_->CreateRenderTargetView(swapChainResources[1], &rtvDesc, rtvHandles[1]);


	// これから書き込むバックバッファのインデックスを取得
	UINT backBufferIndex = swapChain_->GetCurrentBackBufferIndex();


	// TransitionBarrierの設定
	D3D12_RESOURCE_BARRIER barrier{};
	// 今回のバリアはTransition
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	// Noneにしておく
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	// バリアを張る対象のリソース。現在のバックバッファに対して行う
	barrier.Transition.pResource = swapChainResources[backBufferIndex];
	// 遷移前(現在)のResourceState
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	// 遷移後のResourceState
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	// TransitionBarrierを練る
	commandList_->ResourceBarrier(1, &barrier);



	// 描画用のDescriptorHeapの設定
	ID3D12DescriptorHeap* descriptorHeaps[] = {srvHeap_.Get()};
	commandList_->SetDescriptorHeaps(1, descriptorHeaps);

	// 描画先のRTVとDSVを設定する
	D3D12_CPU_DESCRIPTOR_HANDLE dsvHandle = dsvHeap_->GetCPUDescriptorHandleForHeapStart();
	// 描画先のRTVを設定する
	commandList_->OMSetRenderTargets(1, &rtvHandles[backBufferIndex], false, &dsvHandle);
	// 指定した深度で画面全体をクリアする
	commandList_->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	// 指定した色で画面全体をクリアする
	float clearColor[] = {0.1f, 0.25f, 0.5f, 1.0f};
	commandList_->ClearRenderTargetView(rtvHandles[backBufferIndex], clearColor, 0, nullptr);
}

// 深度バッファ
void DirectXCommon::ClearDepthBuffer() {

}

//Fence
void DirectXCommon::CreateFence() {
	HRESULT hr_ = S_FALSE;

	// 初期値0でFenceを作る
	ID3D12Fence* fence = nullptr;
	uint64_t fenceValue = 0;
	hr_ = device_->CreateFence(fenceValue, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	assert(SUCCEEDED(hr_));

	// Fenceのsignalを待つためにイベントを作成する
	HANDLE fenceEvent = CreateEvent(NULL, FALSE, FALSE, NULL);
	assert(fenceEvent != nullptr);
}