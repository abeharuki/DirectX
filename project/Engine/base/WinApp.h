#pragma once
#include <Windows.h>
#include <cstdint>

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
	// Window関連
	HWND hwnd_ = nullptr;   // ウィンドウハンドル
	WNDCLASS wc{}; // ウィンドウクラス
	
};