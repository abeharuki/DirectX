#include "WinApp.h"
#include <string>
#include <d3d12sdklayers.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT
    ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lPaeam);


WinApp* WinApp::GetInstance() {
	static WinApp instance;
	return &instance;
}

// ウィンドウプロシージャ
LRESULT CALLBACK WinApp::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam) {
	
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	
	// メッセージに応じてゲーム固有の処理を行う
	switch (msg) {
		// ウインドウが破棄された
	case WM_DESTROY:
		// osに対して、アプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}
	// 標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}



void WinApp::CreateGameWindow(
    const wchar_t* title, int32_t clientWidth, int32_t clientHeight) {

	CoInitializeEx(0, COINIT_MULTITHREADED);

	
	
     ;
	// ウィンドウプロシージャ
	wc.lpfnWndProc = (WNDPROC)WindowProc;
	// ウインドウクラス名
	wc.lpszClassName = title;
	// インスタンスハンドル
	wc.hInstance = GetModuleHandle(nullptr);
	// カーソル
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);

	// ウインドウクラスを登録する
	RegisterClass(&wc);

	

	// ウインドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc = {0, 0, clientWidth, clientHeight};

	// クライアント領域を元に実際のサイズにwrcを変更してもらう
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	// ウインドウの生成
	hwnd_ = CreateWindow(
	    wc.lpszClassName,
		L"CG2",
		WS_OVERLAPPEDWINDOW, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT,
	    wrc.right - wrc.left, 
		wrc.bottom - wrc.top, 
		nullptr, nullptr, 
		wc.hInstance, 
		nullptr);

#ifdef _DEBUG
	ID3D12Debug1* debugController = nullptr;
	if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
		// デバッグレイヤーを有効化する
		debugController->EnableDebugLayer();
		// さらにGPU側でもチェックを行うようにする
		debugController->SetEnableGPUBasedValidation(TRUE);
	}
#endif

	// ウインドウを表示する
	ShowWindow(hwnd_, SW_SHOW);
}

void WinApp::TerminateGameWindow() {
	// ウィンドウクラスを登録解除
	UnregisterClass(wc.lpszClassName, wc.hInstance);

	// COM 終了
	CoUninitialize();
}

bool WinApp::ProcessMessage() {
	MSG msg{};
	// Windowにメッセージが来てたら最優先で処理させる
	if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	if (msg.message == WM_QUIT) {
        return true;
	}
	return false;
}