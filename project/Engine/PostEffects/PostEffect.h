#pragma once
#include <wrl.h>
#include <d3d12.h>
#include <memory>
#include "StringUtility.h"
#include "Mesh.h"
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
#include "OutLine.h"
#include "Dissolve.h"
#include "GasianBlur.h"
#include "Bloom.h"

/**
 * @struct Grayscale
 * @brief GrayScaleエフェクトの設定を管理する構造体
 */
struct Grayscale {
	int32_t isEnable;
	float padding[3];
};

/**
 * @struct Vignetting
 * @brief Vignettingエフェクトの設定を管理する構造体
 */
struct Vignetting {
	int32_t isEnable;
	Vector3 color;
	float intensity;//明るさ
	float padding[3];
};

/**
 * @struct Gaussian
 * @brief Gaussianエフェクトの設定を管理する構造体
 */
struct Gaussian
{
	int32_t isEnable;
	float kernelSize;
	float padding[2];
};

/**
 * @struct RadialBlur
 * @brief RadialBlurエフェクトの設定を管理する構造体
 */
struct RadialBlur {
	Vector2 center;
	float blurWidth;
	float rotation;
	int32_t isEnble;
	float padding[3];
};

/**
 * @struct Random
 * @brief Randomエフェクトの設定を管理する構造体
 */
struct Random {
	float time;
	int32_t isEnble;
	float padding[2];
};

/**
 * @struct HSVMaterial
 * @brief HSVMaterialエフェクトの設定を管理する構造体
 */
struct HSVMaterial
{
	float hue;
	float saturation;
	float value;
};

/**
 * @struct PostEffects
 * @brief ポストエフェクトに関連する設定データを保持する構造体
 */
struct PostEffects {
	Grayscale grayscal;
	Vignetting vignetting;
	Gaussian gaussian;
	RadialBlur radialBlur;
	Random random;
	HSVMaterial material;
};

/**
 * @class PostEffect
 * @brief ポストエフェクトの管理と適用を行うクラス
 */
class PostEffect
{
public:

    /// <summary>
    /// PostEffect のインスタンスを取得する
    /// </summary>
    /// <returns>PostEffect インスタンス</returns>
    static PostEffect* GetInstance();

    /// <summary>
    /// PostEffect のインスタンスを破棄する
    /// </summary>
    static void Destroy();

    /// <summary>
    /// エフェクトの初期化処理
    /// </summary>
    void Initialize();

    /// <summary>
    /// エフェクトの更新処理
    /// </summary>
    void Update();

    /// <summary>
    /// エフェクトの描画処理
    /// </summary>
    void Draw();

    /// <summary>
    /// エフェクトを適用する処理
    /// </summary>
    void Apply();

    /// <summary>
    /// エフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void Effect(bool flag) { effect_ = flag; }

    /// <summary>
    /// グレースケールエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isGrayscale(bool flag) { postEffectData->grayscal.isEnable = flag; }

    /// <summary>
    /// ビネットエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isVignetting(bool flag) { postEffectData->vignetting.isEnable = flag; }

    /// <summary>
    /// ビネットの強度を設定
    /// </summary>
    /// <param name="intensity">強度</param>
    void VignetteIntensity(float intensity) { postEffectData->vignetting.intensity = intensity; }

    /// <summary>
    /// ビネットの色を設定
    /// </summary>
    /// <param name="color">色</param>
    void VignetteColor(Vector3 color) { postEffectData->vignetting.color = color; }

    /// <summary>
    /// ビネットの設定を変更
    /// </summary>
    /// <param name="vigne">Vignetting 設定</param>
    void Vignette(Vignetting vigne) { postEffectData->vignetting = vigne; }

    /// <summary>
    /// ガウシアンブラーエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isGaussian(bool flag) { postEffectData->gaussian.isEnable = flag; }

    /// <summary>
    /// ガウシアンカーネルサイズを設定
    /// </summary>
    /// <param name="size">カーネルサイズ</param>
    void GaussianKernelSize(float size) { postEffectData->gaussian.kernelSize = size; }

    /// <summary>
    /// アウトラインエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isOutLine(bool flag) { outline_->isOutLine(flag); }

    /// <summary>
    /// アウトラインの強度を設定
    /// </summary>
    /// <param name="value">強度</param>
    void ValueOutLine(float value) { outline_->ValueOutLine(value); }

    /// <summary>
    /// ラジアルブラーの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isRadialBlur(bool flag) { postEffectData->radialBlur.isEnble = flag; }

    /// <summary>
    /// ラジアルブラーの中心を設定
    /// </summary>
    /// <param name="num">中心座標</param>
    void RadialBlurCenter(Vector2 num) { postEffectData->radialBlur.center = { num.x,num.y }; }

    /// <summary>
    /// ラジアルブラーの幅を設定
    /// </summary>
    /// <param name="num">幅</param>
    void RadialBlurWidth(float num) { postEffectData->radialBlur.blurWidth = num; }

    /// <summary>
    /// ラジアルブラーの回転角度を設定
    /// </summary>
    /// <param name="num">回転角度</param>
    void RadialBlurRotation(float num) { postEffectData->radialBlur.rotation = num; }

    /// <summary>
    /// ディゾルブエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isDissolve(bool flag) { dissolve_->isDissolve(flag); }

    /// <summary>
    /// ディゾルブのしきい値を設定
    /// </summary>
    /// <param name="num">しきい値</param>
    void dissolveThreshold(float num) { dissolve_->Threshold(num); }

    /// <summary>
    /// エッジカラーを設定
    /// </summary>
    /// <param name="num">色</param>
    void EdgeColor(Vector3 num) { dissolve_->SetColor(num); }

    /// <summary>
    /// ノイズエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isRandom(bool flag) { postEffectData->random.isEnble = flag; }

    /// <summary>
    /// ガウシアンブラーの幅を設定
    /// </summary>
    /// <param name="num">幅</param>
    void SetBlurWidth(float num) { gasianBlur_->SetWidth(num); }

    /// <summary>
    /// ガウシアンブラーのシグマ値を設定
    /// </summary>
    /// <param name="sigma">シグマ値</param>
    void SetBlurSigma(float sigma) { gasianBlur_->SetSigma(sigma); }

    /// <summary>
    /// ガウシアンブラーの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isGasianBlur(bool flag) { gasianBlur_->isGasianBlur(flag); }

    /// <summary>
    /// ブルームエフェクトの幅を設定
    /// </summary>
    /// <param name="num">幅</param>
    void SetBloomWidth(float num) { bloom_->SetWidth(num); }

    /// <summary>
    /// ブルームエフェクトのシグマ値を設定
    /// </summary>
    /// <param name="num">シグマ値</param>
    void SetBloomSigma(float num) { bloom_->SetSigma(num); }

    /// <summary>
    /// ブルームの光強度を設定
    /// </summary>
    /// <param name="num">光強度</param>
    void BloomLightStrength(float num) { bloom_->lightStrength(num); }

    /// <summary>
    /// ブルームのしきい値を設定
    /// </summary>
    /// <param name="num">しきい値</param>
    void BloomTreshold(float num) { bloom_->Threshold(num); }

    /// <summary>
    /// ブルームエフェクトの有効/無効を設定
    /// </summary>
    /// <param name="flag">有効フラグ</param>
    void isBloom(bool flag) { bloom_->isBloom(flag); }

    /// <summary>
    /// HSVフィルターの設定を行う
    /// </summary>
    /// <param name="hsv">HSVマテリアル</param>
    void SetHsv(HSVMaterial hsv) { postEffectData->material = hsv; }

    /// <summary>
    /// HSV値を個別に設定
    /// </summary>
    /// <param name="hsv">HSVのベクトル値</param>
    void SetHsv(Vector3 hsv) { postEffectData->material.hue = hsv.x; postEffectData->material.saturation = hsv.y; postEffectData->material.value = hsv.y; }

    /// <summary>
    /// HSVの色相を設定
    /// </summary>
    /// <param name="num">色相</param>
    void SetHsvHue(float num) { postEffectData->material.hue = num; }

    /// <summary>
    /// HSVの彩度を設定
    /// </summary>
    /// <param name="num">彩度</param>
    void SetHsvSaturation(float num) { postEffectData->material.saturation = num; }

    /// <summary>
    /// HSVの明度を設定
    /// </summary>
    /// <param name="num">明度</param>
    void SetHsvValue(float num) { postEffectData->material.value = num; }

private:
    PostEffect() = default;
    ~PostEffect() = default;
    PostEffect(const PostEffect&) = delete;
    PostEffect& operator=(const PostEffect&) = delete;

    /// <summary>
    /// リソース作成処理
    /// </summary>
    void CreateResource();

    /// <summary>
    /// グラフィックスパイプラインの初期化
    /// </summary>
    void sPipeline();

private:
    static PostEffect* instance_;  // シングルトンインスタンス

    // 頂点バッファビュー
    D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};

    // ポストエフェクトリソース
    Microsoft::WRL::ComPtr<ID3D12Resource> postEffectResource_;

    // ポストエフェクトのデータ
    PostEffects* postEffectData = nullptr;

    // 個別のエフェクトオブジェクト
    std::unique_ptr<OutLine> outline_;
    std::unique_ptr<Dissolve> dissolve_;
    std::unique_ptr<GasianBlur> gasianBlur_;
    std::unique_ptr<Bloom> bloom_;

    // ルートシグネチャ
    Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature_;

    // パイプラインステートオブジェクト
    Microsoft::WRL::ComPtr<ID3D12PipelineState> sPipelineState_;

    // シェーダーバイナリデータ
    Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob_;
    Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob_;

    // エフェクトの有効/無効フラグ
    bool effect_ = true;
};
