#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
#include "Math/math.h"
#include <fstream>
#include <sstream>
#include <dxcapi.h>
#include <DirectXTex.h>
#include <d3dx12.h>
#include "GraphicsPipeline.h"
#include "ModelManager.h"
#include "base/ColorBuffer.h"

/**
 * @struct Blur
 * @brief GasianBlurエフェクトの設定を管理する構造体
 */
struct Blur {
	float stepWidth;
	float sigma;
	int32_t isEnble;
};

/**
 * @class GasianBlur
 * @brief GasianBlurエフェクトを適用するクラス
 */
class GasianBlur {
public:

    /// <summary>
    /// ガウシアンブラーの初期化処理
    /// </summary>
    void Initialize();

    /// <summary>
    /// 描画処理
    /// </summary>
    /// <param name="srvHandle">シェーダリソースビューのハンドル</param>
    void Draw(DescriptorHandle srvHandle);

    /// <summary>
    /// 描画前のセットアップ処理
    /// </summary>
    void PreDraw();

    /// <summary>
    /// 描画後の後処理
    /// </summary>
    void PostDraw();

    /// <summary>
    /// 現在のシェーダリソースビュー(SRV)ハンドルを取得
    /// </summary>
    /// <returns>シェーダリソースビューのハンドル</returns>
    const DescriptorHandle& GetHandle() { return colorBuffer_->GetSRVHandle(); }

    /// <summary>
    /// ガウシアンブラーのステップ幅を設定
    /// </summary>
    /// <param name="num">ステップ幅</param>
    void SetWidth(float num) { blurData->stepWidth = num; }

    /// <summary>
    /// ガウシアンブラーのシグマ値を設定
    /// </summary>
    /// <param name="sigma">シグマ値</param>
    void SetSigma(float sigma) { blurData->sigma = sigma; }

    /// <summary>
    /// ガウシアンブラーの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isGasianBlur(bool flag) { blurData->isEnble = flag; }

private:

    /// <summary>
    /// リソース作成処理
    /// </summary>
    void CreateResource();

    /// <summary>
    /// グラフィックスパイプラインの初期化
    /// </summary>
    void sPipeline();

private:

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    // リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    // ガウシアンブラー用のデータ構造
    Blur* blurData = nullptr;

    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // パイプラインステートオブジェクト
    Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

    // 頂点シェーダのバイナリデータ
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;

    // ピクセルシェーダのバイナリデータ
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    // カラーバッファ
    std::unique_ptr<ColorBuffer> colorBuffer_ = nullptr;

    // クリアカラー
    float clearColor_[4]{ 0.1f, 0.25f, 0.5f, 1.0f };

    // マスクテクスチャID
    uint32_t texture_;
};
