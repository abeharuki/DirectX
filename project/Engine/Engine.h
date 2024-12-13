#pragma once
#include "DirectXCommon.h"
#include "WinApp.h"
#include "base/GpuResource.h"

#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")

/**
 * @class Engine
 * @brief ゲームエンジンの中心となるクラス
 */
class Engine {
public:
    /// <summary>
    /// シングルトンパターンでインスタンスを取得
    /// </summary>
    /// <returns>Engineインスタンス</returns>
    static Engine* GetInstance();

    /// <summary>
    /// エンジンの初期化
    /// </summary>
    /// <param name="title">ウィンドウのタイトル</param>
    /// <param name="width">ウィンドウの横幅</param>
    /// <param name="height">ウィンドウの縦幅</param>
    static void Initialize(const wchar_t* title, int width, int height);

    /// <summary>
    /// エンジンの後始末
    /// </summary>
    static void Finalize();

    /// <summary>
    /// メッセージ処理（Windowsメッセージループ）
    /// </summary>
    /// <returns>処理が成功したかどうか</returns>
    static bool ProcessMessage();

    /// <summary>
    /// フレーム開始時に呼ばれる
    /// </summary>
    static void BeginFrame();

    /// <summary>
    /// フレーム終了時に呼ばれる
    /// </summary>
    static void EndFrame();

    /// <summary>
    /// 描画後の処理（ポストプロセス等）
    /// </summary>
    static void PostDraw();

    /// <summary>
    /// 描画前の処理（前処理）
    /// </summary>
    static void PreDraw();

    /// <summary>
    /// レンダリング時の前処理
    /// </summary>
    static void RenderPreDraw();

    /// <summary>
    /// レンダリング時の後処理
    /// </summary>
    static void RenderPostDraw();

    /// <summary>
    /// 深度バッファの前処理
    /// </summary>
    static void DepthPreDraw();

    /// <summary>
    /// 深度バッファの後処理
    /// </summary>
    static void DepthPostDraw();

    /// <summary>
    /// UAVリソースのバリア処理
    /// </summary>
    /// <param name="resource">GPUリソース</param>
    static void UAVBarrier(GpuResource& resource);

    /// <summary>
    /// CPUディスクリプタハンドルを取得
    /// </summary>
    /// <param name="descriptorheap">ディスクリプタヒープ</param>
    /// <param name="descriptorSize">ディスクリプタのサイズ</param>
    /// <param name="index">インデックス</param>
    /// <returns>CPUディスクリプタハンドル</returns>
    static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(
        ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);

    /// <summary>
    /// GPUディスクリプタハンドルを取得
    /// </summary>
    /// <param name="descriptorheap">ディスクリプタヒープ</param>
    /// <param name="descriptorSize">ディスクリプタのサイズ</param>
    /// <param name="index">インデックス</param>
    /// <returns>GPUディスクリプタハンドル</returns>
    static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(
        ID3D12DescriptorHeap* descriptorheap, uint32_t descriptorSize, uint32_t index);

public:
    /// <summary>
    /// デバイスを取得
    /// </summary>
    /// <returns>D3D12デバイス</returns>
    static Microsoft::WRL::ComPtr<ID3D12Device> GetDevice();

    /// <summary>
    /// グラフィックスコマンドリストを取得
    /// </summary>
    /// <returns>D3D12グラフィックスコマンドリスト</returns>
    static Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> GetList();

    /// <summary>
    /// SRV（Shader Resource View）用のディスクリプタヒープを取得
    /// </summary>
    /// <returns>SRV用ディスクリプタヒープ</returns>
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetSRV();

    /// <summary>
    /// RTV（Render Target View）用のディスクリプタヒープを取得
    /// </summary>
    /// <returns>RTV用ディスクリプタヒープ</returns>
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetRTV();

    /// <summary>
    /// DSV（Depth Stencil View）用のディスクリプタヒープを取得
    /// </summary>
    /// <returns>DSV用ディスクリプタヒープ</returns>
    static Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDSV();

    /// <summary>
    /// 現在のSRV（Shader Resource View）のディスクリプタハンドルを取得
    /// </summary>
    /// <returns>SRVディスクリプタハンドル</returns>
    const DescriptorHandle& GetHandle();

    /// <summary>
    /// 現在のDSV（Depth Stencil View）のディスクリプタハンドルを取得
    /// </summary>
    /// <returns>DSVディスクリプタハンドル</returns>
    const DescriptorHandle& GetDepthHandle();

    /// <summary>
    /// GPUリソースの状態を遷移させる
    /// </summary>
    /// <param name="resource">対象のGPUリソース</param>
    /// <param name="newState">遷移先の状態</param>
    void TransitionResource(GpuResource& resource, D3D12_RESOURCE_STATES newState);

    // ゲーム時間（秒）
    static float gameTime;
};
