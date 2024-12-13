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
#include "Engine.h"
#include "TextureManeger.h"
#include <DirectXTex.h>
#include <d3dx12.h>
#include "WorldTransform.h"
#include "ViewProjection.h"
#include "GraphicsPipeline.h"
#include "ModelManager.h"
#include "base/ColorBuffer.h"

/**
 * @struct OutLineStyle
 * @brief OutLineエフェクトの設定を管理する構造体
 */
struct OutLineStyle {
	
	int32_t isEnable;
	float differenceValue;	
	float padding1[2];
	Matrix4x4 projectionInverse;
	
};

/**
 * @class outLine
 * @brief outLineエフェクトを適用するクラス
 */
class OutLine
{
public:

    /// <summary>
    /// アウトライン処理の初期化
    /// </summary>
    void Initialize();

    /// <summary>
    /// 描画処理
    /// </summary>
    void Draw();

    /// <summary>
    /// 描画前のセットアップ処理
    /// </summary>
    void PreDraw();

    /// <summary>
    /// 描画後の後処理
    /// </summary>
    void PostDraw();

    /// <summary>
    /// アウトラインエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isOutLine(bool flag) { outLineData->isEnable = flag; }

    /// <summary>
    /// アウトラインの差分値を設定
    /// </summary>
    /// <param name="value">差分値</param>
    void ValueOutLine(float value) { outLineData->differenceValue = value; }

    /// <summary>
    /// ビュー射影行列を設定
    /// </summary>
    /// <param name="viewProjection">ビュー射影行列</param>
    void SetViewProjection(ViewProjection& viewProjection) {
        viewProjection_ = viewProjection;
        outLineData->projectionInverse = Math::Inverse(viewProjection_.matProjection);
    }

    /// <summary>
    /// 現在のシェーダリソースビュー(SRV)ハンドルを取得
    /// </summary>
    /// <returns>シェーダリソースビューのハンドル</returns>
    const DescriptorHandle& GetHandle() { return colorBuffer_->GetSRVHandle(); }

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

    // ビュー射影行列データ
    ViewProjection viewProjection_;

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    // ポストエフェクト用リソース
    Microsoft::WRL::ComPtr<ID3D12Resource> postEffectResource_;

    // アウトラインエフェクトデータ構造
    OutLineStyle* outLineData = nullptr;

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
};
