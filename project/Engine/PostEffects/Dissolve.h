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
 * @struct DissolveStyle
 * @brief Dissolveエフェクトの設定を管理する構造体
 */
struct DissolveStyle {
	Matrix4x4 uvTransform;
	float threshold;
	Vector3 edgeColor;
	int32_t isGradient;
	int32_t isEnble;
	float pading[2];
};

/**
 * @class Dissolve
 * @brief Dissolveエフェクトを適用するクラス
 */
class Dissolve {
public:

    /// <summary>
    /// ディゾルブエフェクトの初期化
    /// 必要なリソースやパイプラインを設定
    /// </summary>
    void Initialize();

    /// <summary>
    /// 描画処理を実行
    /// 指定されたシェーダリソースビュー(SRV)ハンドルを使用
    /// </summary>
    /// <param name="srvHandle">シェーダリソースビューのハンドル</param>
    void Draw(DescriptorHandle srvHandle);

    /// <summary>
    /// 描画前の準備処理を実行
    /// 状態のセットアップ
    /// </summary>
    void PreDraw();

    /// <summary>
    /// 描画後の後処理を実行します。
    /// 状態のリセットや後続処理の準備
    /// </summary>
    void PostDraw();

    /// <summary>
    /// 現在のディゾルブのシェーダリソースビュー(SRV)ハンドルを取得
    /// </summary>
    /// <returns>シェーダリソースビューのハンドル</returns>
    const DescriptorHandle& GetHandle() { return colorBuffer_->GetSRVHandle(); }

    /// <summary>
    /// ディゾルブエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">ディゾルブエフェクトの有効フラグ</param>
    void isDissolve(bool flag) { dissolveData->isEnble = flag; }

    /// <summary>
    /// ディゾルブの閾値を設定
    /// </summary>
    /// <param name="num">閾値</param>
    void Threshold(float num) { dissolveData->threshold = num; }

    /// <summary>
    /// エッジ部分のカラーを設定
    /// </summary>
    /// <param name="num">カラー値 (Vector3形式)</param>
    void SetColor(Vector3 num) { dissolveData->edgeColor = num; }

private:

    /// <summary>
    /// 必要なリソースを作成
    /// バッファやテクスチャなどを初期化
    /// </summary>
    void CreateResource();

    /// <summary>
    /// グラフィックスパイプラインの初期化
    /// シェーダやステートの設定を実行
    /// </summary>
    void sPipeline();

    /// <summary>
    /// マスクテクスチャを読み込み
    /// ディゾルブエフェクトに使用
    /// </summary>
    /// <param name="texturePath">テクスチャファイルのパス</param>
    void LoadTexture(const std::string& texturePath);

private:

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    // 描画リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> resource_;

    // ディゾルブのデータ構造
    DissolveStyle* dissolveData = nullptr;

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
