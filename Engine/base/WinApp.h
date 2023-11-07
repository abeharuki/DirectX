#pragma once
#include <Windows.h>
#include <cstdint>
#include <wrl.h>
#include <d3d12sdklayers.h>
#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>
extern IMGUI_IMPL_API LRESULT
    ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lPaeam);

class WinApp {
public:
	// ウィンドウサイズ
	static const int kWindowWidth = 1280; // 横幅
	static const int kWindowHeight = 720; // 縦幅
	

	static WinApp* GetInstance();

	static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	void CreateGameWindow(
	    const wchar_t* title = L"DirectXGame", int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

	void TerminateGameWindow();

	bool ProcessMessage();

public: // ゲッター
	static const HWND GetHwnd() { return GetInstance()->hwnd_; };
	static const WNDCLASS GetWc() { return GetInstance()->wc; };

private: // メンバ関数
	WinApp() = default;
	~WinApp() = default;
	WinApp(const WinApp&) = delete;
	const WinApp& operator=(const WinApp&) = delete;

private: // メンバ変数
	Microsoft::WRL::ComPtr<ID3D12Debug1> debugController_;
	// Window関連
	HWND hwnd_ = nullptr;   // ウィンドウハンドル
	WNDCLASS wc{}; // ウィンドウクラス
	
};