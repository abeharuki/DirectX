#pragma once
#include <Windows.h>
#include <cstdint>

/**
 * @class WinApp
 * @brief Windowsアプリケーションのウィンドウ管理を行うクラス
 */
class WinApp {
public:
    // ウィンドウサイズ
    static const int kWindowWidth = 1280;  ///< 横幅
    static const int kWindowHeight = 720;  ///< 縦幅

    /// <summary>
    /// WinAppのインスタンスを取得
    /// </summary>
    /// <returns>WinAppのインスタンス</returns>
    static WinApp* GetInstance();

    /// <summary>
    /// ウィンドウのメッセージ処理を行う
    /// </summary>
    /// <param name="hwnd">ウィンドウハンドル</param>
    /// <param name="msg">メッセージ</param>
    /// <param name="wparam">メッセージ固有のパラメータ</param>
    /// <param name="lparam">メッセージ固有のパラメータ</param>
    /// <returns>結果のLRESULT</returns>
    static LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

    /// <summary>
    /// ゲームウィンドウを作成
    /// </summary>
    /// <param name="title">ウィンドウのタイトル</param>
    /// <param name="clientWidth">ウィンドウの幅</param>
    /// <param name="clientHeight">ウィンドウの高さ</param>
    void CreateGameWindow(
        const wchar_t* title = L"DirectXGame", int32_t clientWidth = kWindowWidth, int32_t clientHeight = kWindowHeight);

    /// <summary>
    /// ゲームウィンドウを終了
    /// </summary>
    void TerminateGameWindow();

    /// <summary>
    /// メッセージの処理
    /// </summary>
    /// <returns>メッセージの処理結果</returns>
    bool ProcessMessage();

public: // ゲッター
    /// <summary>
    /// ウィンドウハンドルを取得
    /// </summary>
    /// <returns>ウィンドウハンドル</returns>
    static const HWND GetHwnd() { return GetInstance()->hwnd_; };

    /// <summary>
    /// ウィンドウクラスを取得
    /// </summary>
    /// <returns>ウィンドウクラス</returns>
    static const WNDCLASS GetWc() { return GetInstance()->wc; };

private: // メンバ関数
    /// <summary>
    /// コンストラクタ（デフォルト）
    /// </summary>
    WinApp() = default;

    /// <summary>
    /// デストラクタ
    /// </summary>
    ~WinApp() = default;

    /// <summary>
    /// コピーコンストラクタ（削除）
    /// </summary>
    WinApp(const WinApp&) = delete;

    /// <summary>
    /// コピー代入演算子（削除）
    /// </summary>
    const WinApp& operator=(const WinApp&) = delete;

private: // メンバ変数
    // Window関連
    HWND hwnd_ = nullptr;   ///< ウィンドウハンドル
    WNDCLASS wc{};          ///< ウィンドウクラス
};