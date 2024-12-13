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
 * @struct BloomStyle
 * @brief Bloomエフェクトの設定を管理する構造体
 */
struct BloomStyle {
	float stepWidth;
	float sigma;
	float lightStrength;
	float bloomThreshold;
	int32_t isEnble;
};

/**
 * @class Bloom
 * @brief Bloomエフェクトを適用するクラス
 */
class Bloom {
public:

    /// <summary>
    /// ブルームエフェクトの初期化
    /// 必要なリソースやパイプラインを設定
    /// </summary>
    void Initialize();

    /// <summary>
    /// 描画処理
    /// 指定されたシェーダリソースビュー(SRV)ハンドルを使用
    /// </summary>
    /// <param name="srvHandle">シェーダリソースビューのハンドル</param>
    void Draw(DescriptorHandle srvHandle);

    /// <summary>
    /// 描画前の処理
    /// 必要な状態のセットアップ
    /// </summary>
    void PreDraw();

    /// <summary>
    /// 描画後の処理
    /// 後処理や状態のリセットを実行
    /// </summary>
    void PostDraw();

    /// <summary>
    /// 現在のブルームのシェーダリソースビュー(SRV)ハンドルを取得
    /// </summary>
    /// <returns>シェーダリソースビューのハンドル</returns>
    const DescriptorHandle& GetHandle() { return colorBuffer_->GetSRVHandle(); }

    /// <summary>
    /// テクスチャサイズを設定
    /// </summary>
    /// <param name="num">新しいテクスチャサイズ</param>
    void SetWidth(float num) { bloomData->stepWidth = num; }

    /// <summary>
    /// ガウスぼかしのシグマ値を設定
    /// </summary>
    /// <param name="sigma">シグマ値</param>
    void SetSigma(float sigma) { bloomData->sigma = sigma; }

    /// <summary>
    /// ライトの強度を設定
    /// </summary>
    /// <param name="strength">ライトの強度</param>
    void lightStrength(float strength) { bloomData->lightStrength = strength; }

    /// <summary>
    /// ブルームのしきい値を設定
    /// </summary>
    /// <param name="threshold">しきい値</param>
    void Threshold(float threshold) { bloomData->bloomThreshold = threshold; }

    /// <summary>
    /// ブルームエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">ブルーム有効フラグ</param>
    void isBloom(bool flag) { bloomData->isEnble = flag; }

private:

    /// <summary>
    /// 必要なリソースを作成
    /// バッファやテクスチャなどの初期化を実行
    /// </summary>
    void CreateResource();

    /// <summary>
    /// グラフィックスパイプラインの初期化
    /// シェーダやステートの設定を実行
    /// </summary>
    void sPipeline();

private:

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    // 描画リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    // ブルームのデータ構造
    BloomStyle* bloomData = nullptr;

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

    // テクスチャID
    uint32_t texture_;
};

