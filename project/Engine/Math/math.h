#pragma once
#include <cassert>
#include <format>
#include <vector>
#include "Quaternion.h"
#include <map>


// 2Dベクトル
struct Vector2 final {
    float x; // x座標
    float y; // y座標
};

// 3Dベクトル
struct Vector3 final {
    float x; // x座標
    float y; // y座標
    float z; // z座標

    // スカラー倍演算子
    Vector3& operator*=(const float s) {
        x *= s;
        y *= s;
        z *= s;
        return *this;
    }

    // ベクトルの引き算演算子
    Vector3& operator-=(const Vector3& v) {
        x -= v.x;
        y -= v.y;
        z -= v.z;
        return *this;
    }

    // ベクトルの加算演算子
    Vector3& operator+=(const Vector3& v) {
        x += v.x;
        y += v.y;
        z += v.z;
        return *this;
    }

    // スカラーで割る演算子
    Vector3& operator/=(const float s) {
        x /= s;
        y /= s;
        z /= s;
        return *this;
    }
};

// 4Dベクトル
struct Vector4 final {
    float x; // x座標
    float y; // y座標
    float z; // z座標
    float w; // w座標
};

// 頂点構造体
struct Vertex {
    Vector3 position; // 頂点位置
    Vector4 color;    // 頂点色
};

// 3x3行列
struct Matrix3x3 final {
    float m[3][3]; // 行列データ
};

// 4x4行列
struct Matrix4x4 final {
    float m[4][4]; // 行列データ

    // 行列同士の比較（等しいかどうか）
    bool operator==(const Matrix4x4& rhs) const {
        for (int i = 0; i < 4; ++i) {
            for (int j = 0; j < 4; ++j) {
                if (m[i][j] != rhs.m[i][j]) {
                    return false;
                }
            }
        }
        return true;
    }

    // 行列同士の比較（異なるかどうか）
    bool operator!=(const Matrix4x4& rhs) const {
        return !(*this == rhs);
    }
};

// Axis-Aligned Bounding Box (AABB) - 軸に平行な境界ボックス
struct AABB {
    Vector3 min; // 最小点
    Vector3 max; // 最大点
};

// Oriented Bounding Box (OBB) - 任意の方向を持つ境界ボックス
struct OBB {
    Vector3 center; // 中心点
    Vector3 orientations[3]; // 座標軸（正規化・直交必須）
    Vector3 size; // 座標軸方向の半分の長さ（中心から面までの距離）
};

// 変換データ
struct Transform {
    Vector3 scale;    // スケール
    Vector3 rotate;   // 回転
    Vector3 translate; // 平行移動
};

// 頂点データ
struct VertexData {
    Vector4 position; // 頂点位置
    Vector2 texcoord; // テクスチャ座標
    Vector3 normal;   // 法線
};

// 球体
struct sphere {
    Vector3 center; // 中心点
    float radius;   // 半径
};

// 円
struct Circle {
    Vector2 center; // 中心点
    float radius;   // 半径
};

// 色データ（RGB + アルファ）
struct Color {
    Vector3 rgb; // RGBカラー
    float a;     // アルファ（透明度）
};

// マテリアルデータ
struct MaterialD {
    Color color;          // カラー
    int32_t enableLighting; // ライティングの有効化フラグ
    float padding[3];     // パディング
    Matrix4x4 uvTransform; // UV変換行列
    float shininess;       // 光沢度
};

// クォータニオンを使用した変換データ
struct QuaternionTransform {
    Vector3 scale;    // スケール
    Quaternion rotate; // 回転（クォータニオン）
    Vector3 translate; // 平行移動
};

// ノード構造体（シーンツリー）
struct Node {
    QuaternionTransform transform; // 変換
    Matrix4x4 localMatrix{}; // ローカル行列
    std::string name; // ノード名
    std::vector<Node> children; // 子ノードのリスト
};

// メッシュデータ
struct MeshData {
    std::vector<VertexData> vertices; // 頂点データ
    std::vector<uint32_t> indices;    // インデックスデータ
    uint32_t materialIndex; // マテリアルインデックス
};

// マテリアルデータ（テクスチャファイルや色など）
struct MaterialData {
    std::string textureFilePath; // テクスチャファイルのパス
    Color diffuseColor;         // 拡散色
};

// 頂点のウェイトデータ
struct VertexWeightData {
    float weight;       // ウェイト
    uint32_t vertexIndex; // 対応する頂点のインデックス
};

// ジョイントのウェイトデータ
struct JointWeightData {
    Matrix4x4 inverseBindPoseMatrix; // 逆バインドポーズ行列
    std::vector<VertexWeightData> vertexWeights; // 頂点のウェイトデータ
};

// モデルデータ
struct ModelData {
    std::map<std::string, JointWeightData> skinClusterData; // スキンクラスターのデータ
    MeshData meshData;   // メッシュデータ
    MaterialData material; // マテリアルデータ
    Node rootNode;        // ルートノード
};

// 変換行列
struct TransformationMatrix {
    Matrix4x4 WVP;    // ワールド・ビュー・プロジェクション行列
    Matrix4x4 World;  // ワールド行列
};

// カメラデータ（GPU向け）
struct CameraForGPU {
    Vector3 worldPos; // ワールド空間でのカメラの位置
};

// ライト構造体（方向光）
struct DirectionLight {
    Vector4 color;     // ライトの色
    Vector3 direction; // ライトの向き
    float intensity;   // 輝度
    bool isEnable_ = false; // ライトの有効化フラグ
    float padding[3];  // パディング
};

// 点光源データ
struct PointLight {
    Vector4 color_;    // ライトの色
    Vector3 position_; // ライトの位置
    float intensity_;  // 輝度
    float radius_;     // ライトの届く距離
    float decay_;      // 減衰
    bool isEnable_ = false; // ライトの有効化フラグ
    float padding;     // パディング
};

// スポットライトデータ
struct SpotLight {
    Vector4 color_;     // ライトの色
    Vector3 position_;  // ライトの位置
    float intensity_;   // 輝度
    Vector3 direction_; // ライトの方向
    float distance_;    // ライトの届く距離
    float decay_;       // 減衰
    float cosAngle_;    // 光の角度
    bool isEnable_ = false; // ライトの有効化フラグ
    float padding;      // パディング
};

// 環境光データ
struct Environment {
    float environment; // 環境光
    bool isEnble_ = false; // 環境光の有効化フラグ
    float padding[2]; // パディング
};

// パーティクルデータ
struct Particle {
    Transform transform; // 変換データ
    Vector3 velocity;    // 速度
    Vector4 color;       // 色
    float lifeTime;      // 寿命
    float currentTime;   // 現在の時間
};

// パーティクルシステムデータ（シェーダー用）
struct ParticleCS {
    Vector3 translate;    // 平行移動
    Vector3 scale;        // スケール
    float lifeTime;       // 寿命
    Vector3 velocity;     // 速度
    float currentTime;    // 現在の時間
    Vector4 color;        // 色
    float initialAlpha;   // 初期アルファ
    float endAlpha;       // 終了時アルファ
    Vector3 initialScale; // 初期スケール
    Vector3 endScale;     // 終了時スケール
};

///GPUパーティクルデータ
struct ParticleForGPU {
	Matrix4x4 WVP;
	Matrix4x4 World;
	Vector4 color;
};

//エミッター
struct Emitter {
	Transform transform;
	uint32_t count;      // 発生数　発生頻度秒に何個出すか
	float frequency;     // 発生頻度
	float frequencyTime; // 頻度用時刻　0で初期化
};

//風フィールド
struct AccelerationField {
	Vector3 acceleration;//加速度
	Vector3 translate;//位置
	Vector3 min;//最小範囲
	Vector3 max;//最大範囲
};

//重力フィールド
struct GravityField
{
	Vector3 translate;//位置
	Vector3 min;//最小範囲
	Vector3 max;//最大範囲
	float strength;//重力の強さ
	float stopDistance;//動きを止める中心点からの距離
};

// 描画時に必要なカメラ関連の情報を保持する構造体
struct PerView {
    Matrix4x4 view;              // ビュー行列：カメラの視点や向きを表す行列
    Matrix4x4 projection;        // 投影行列：透視投影または正射影の設定を表す行列
    Matrix4x4 billboardMatrix;   // ビルボード行列：オブジェクトを常にカメラに正面を向かせるための行列
};

// フレームごとの経過時間を管理するための構造体
struct PerFrame {
    float time;          // ゲーム開始からの経過時間（秒単位）
    float deltaTime;     // 現在のフレームと前のフレームの間隔（秒単位）
};

/**
 * @class Math.h
 * @brief 数学的な計算やベクトル・行列操作を提供するクラス
 */
 /**
  * @class Math
  * @brief 数学的な計算やベクトル・行列操作を提供するユーティリティクラス
  */
class Math {
public:

    /**
     * @brief 値が指定範囲内に収まっているかを判定します。
     * @param value 判定する値
     * @param range 中心値
     * @param sub 許容される範囲の幅
     * @return 範囲内であればtrue、そうでなければfalse
     */
    static bool isWithinRange(float value, float range, float sub);

    /**
     * @brief ベクトルの内積を計算します。
     * @param v1 ベクトル1
     * @param v2 ベクトル2
     * @return 内積の結果
     */
    static float Dot(const Vector3& v1, const Vector3& v2);

    /**
     * @brief 3D空間での円運動を計算します。
     * @param posX X座標
     * @param posZ Z座標
     * @param centerX 円の中心X座標
     * @param centerZ 円の中心Z座標
     * @param radius 円の半径
     * @param angle 現在の角度（更新されます）
     * @param speed 回転速度
     */
    static void UpdateCircularMotion3D(float& posX, float& posZ, float centerX, float centerZ, float radius, float& angle, float speed);

    /**
     * @brief 2つの円の衝突判定を行います。
     * @param circleA 円A
     * @param circleB 円B
     * @return 衝突していればtrue、していなければfalse
     */
    static bool CircleColiision(Circle circleA, Circle circleB);

    /**
     * @brief ベクトルを正規化します。
     * @param v 入力ベクトル
     * @return 正規化されたベクトル
     */
    static Vector3 Normalize(const Vector3& v);

    // ベクトル演算（加算、減算、スカラー倍）
    static Vector3 Add(const Vector3& v1, const Vector3& v2);
    static Vector3 Subract(const Vector3& v1, const Vector3& v2);
    static Vector3 Multiply(float scalar, const Vector3& v);

    // 行列演算（加算、減算、スカラー倍、転置）
    static Matrix4x4 Add(const Matrix4x4& m1, const Matrix4x4& m2);
    static Matrix4x4 Subract(const Matrix4x4& m1, const Matrix4x4& m2);
    static Matrix4x4 Multiply(const Matrix4x4& m1, const Matrix4x4& m2);
    static Matrix4x4 Transpose(const Matrix4x4& m);

    /**
     * @brief ベクトルに行列を適用します。
     * @param vector 入力ベクトル
     * @param matrix 変換行列
     * @return 変換されたベクトル
     */
    static Vector3 TransformNormal(const Vector3& vector, const Matrix4x4& matrix);

    // 行列生成関数（単位行列、回転、スケール、平行移動、透視投影など）
    static Matrix4x4 MakeIdentity4x4();
    static Matrix4x4 MakeRotateXMatrix(float theta = 0);
    static Matrix4x4 MakeRotateYMatrix(float theta = 0);
    static Matrix4x4 MakeRotateZMatrix(float theta = 0);
    static Matrix4x4 MakeRotateMatrix(const Quaternion& quaternion);
    static Matrix4x4 MakeScaleMatrix(Vector3 scale);
    static Matrix4x4 MakeTranslateMatrix(Vector3 translate);
    static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Vector3& rotate, const Vector3& translate);
    static Matrix4x4 MakeAffineMatrix(const Vector3& scale, const Quaternion& quaternion, const Vector3& translation);
    static Matrix4x4 MakeAffineRotateMatrix(const Vector3& scale, const Matrix4x4& rotate, const Vector3& translate);
    static Matrix4x4 MakePerspectiverFovMatrix(float fovY, float aspectRatio, float nearClip, float farClip);
    static Matrix4x4 MakeOrthographicMatrix(float left, float top, float right, float bottom, float nearClip, float farClip);
    static Matrix4x4 MakeViewportMatrix(float left, float top, float width, float height, float minDepth, float maxDepth);
    static Matrix4x4 Inverse(const Matrix4x4& m);

    // ベクトル操作（長さ、線形補間、クロス積）
    static float Length(const Vector3& v);
    static Vector3 Lerp(const Vector3& p0, const Vector3& p1, float t);
    static float LerpShortAngle(float a, float b, float t);
    static Vector3 Cross(const Vector3& v1, const Vector3& v2);

    // 衝突判定
    static bool IsAABBCollision(const Vector3& translate1, const Vector3 size1, const Vector3& translate2, const Vector3 size2);

    // AABBとOBBの変換・押し出し
    static OBB ConvertAABBToOBB(AABB& aabb);
    static AABB ConvertOBBToAABB(OBB& obb);
    static Vector3 PushOutAABB(const Vector3& subjectTranslate, const AABB& subjecAABB, const Vector3& translate, const AABB& aabb);
    static Vector3 PushOutAABBOBB(const Vector3& aabbTranslate, const AABB& aabb, const Vector3& obbTranslate, const OBB& obb);
    static Vector3 PushOutOBBAABB(const Vector3& aabbTranslate, const AABB& aabb, const Vector3& obbTranslate, const OBB& obb);

    /**
     * @brief クォータニオンの球面線形補間を行います。
     * @param q0 補間開始のクォータニオン
     * @param q1 補間終了のクォータニオン
     * @param t 補間係数（0.0～1.0）
     * @return 補間されたクォータニオン
     */
    static Quaternion Slerp(const Quaternion& q0, const Quaternion& q1, float t);
};


/*--------------------演算子オーバーロード---------------------------*/
// 二項演算子
Vector3 operator+(const Vector3& v1, const Vector3& v2);
Vector3 operator+(const Vector3& v1, float s);
Vector3 operator-(const Vector3& v1, const Vector3& v2);
Vector3 operator-(const Vector3& v1, float s);
Vector3 operator*(float s, const Vector3& v2);
Vector3 operator*(const Vector3& v, float s);
Vector3 operator/(const Vector3& v, float s);
Vector3 operator*(const Vector3& v1, const Vector3 v2);
Matrix4x4 operator+(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator-(const Matrix4x4& m1, const Matrix4x4& m2);
Matrix4x4 operator*(const Matrix4x4& m1, const Matrix4x4& m2);

// 単項演算子
Vector3 operator-(const Vector3& v);
Vector3 operator+(const Vector3& v);

