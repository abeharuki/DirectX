#pragma once
#include "3d/WorldTransform.h"
#include "Math/math.h"
#include <LineBox.h>

/**
 * @enum Primitive
 * @brief 衝突判定に使用するプリミティブの種類を表す列挙型
 */
enum class Primitive {
	kAABB,
	kOBB,
	kSphere,
};

/**
 * @class Collider
 * @brief 基本的な衝突判定用のクラス
 */
class Collider
{
public:
    /// <summary>
    /// 衝突した際に呼ばれるコールバック関数
    /// </summary>
    /// <param name="collider">衝突相手のCollider</param>
    virtual void OnCollision(Collider* collider) = 0;

    /// <summary>
    /// コライダーのワールド座標を取得
    /// </summary>
    /// <returns>ワールド座標</returns>
    virtual const Vector3 GetWorldPosition() const = 0;

    /// <summary>
    /// コライダーのワールドトランスフォームを取得
    /// </summary>
    /// <returns>ワールドトランスフォーム</returns>
    virtual const WorldTransform& GetWorldTransform() const = 0;

    /// <summary>
    /// コライダーの半径を取得
    /// </summary>
    /// <returns>半径</returns>
    const float GetRadius() const { return radius_; }

    /// <summary>
    /// コライダーの半径を設定
    /// </summary>
    /// <param name="radius">半径</param>
    void SetRadius(float radius) { radius_ = radius; priType = Primitive::kSphere; }

    /// <summary>
    /// コライダーのAABB（軸平行境界ボックス）を取得
    /// </summary>
    /// <returns>AABB</returns>
    const AABB& GetAABB() const { return aabb_; }

    /// <summary>
    /// コライダーのAABB（軸平行境界ボックス）を設定
    /// </summary>
    /// <param name="aabb">AABB</param>
    void SetAABB(AABB& aabb) { aabb_ = aabb; priType = Primitive::kAABB; SetBounds(); }

    /// <summary>
    /// コライダーのOBB（方向付き境界ボックス）を取得
    /// </summary>
    /// <returns>OBB</returns>
    const OBB& GetOBB() const { return obb_; }

    /// <summary>
    /// コライダーのOBB（方向付き境界ボックス）を設定
    /// </summary>
    /// <param name="obb">OBB</param>
    void SetOBB(OBB& obb) { obb_ = obb; priType = Primitive::kOBB; SetBounds(); }

    /// <summary>
    /// コライダーのサイズを更新（OBBの場合）
    /// </summary>
    /// <param name="size">新しいサイズ</param>
    void SizeUpdata(Vector3 size) {
        obb_.size = size;
        lineBox_->Update(Math::ConvertOBBToAABB(obb_));
    }

    /// <summary>
    /// 衝突属性を取得
    /// </summary>
    /// <returns>衝突属性</returns>
    const uint32_t GetCollisionAttribute() const { return collisionAttribute_; }

    /// <summary>
    /// 衝突属性を設定
    /// </summary>
    /// <param name="collisionAttribute">衝突属性</param>
    void SetCollisionAttribute(uint32_t collisionAttribute) { collisionAttribute_ = collisionAttribute; }

    /// <summary>
    /// 衝突マスクを取得
    /// </summary>
    /// <returns>衝突マスク</returns>
    const uint32_t GetCollisionMask() const { return collisionMask_; }

    /// <summary>
    /// 衝突マスクを設定
    /// </summary>
    /// <param name="collisionMask">衝突マスク</param>
    void SetCollisionMask(uint32_t collisionMask) { collisionMask_ = collisionMask; }

    /// <summary>
    /// 衝突形状を取得（球、AABB、OBBなど）
    /// </summary>
    /// <returns>衝突形状</returns>
    const uint32_t GetCollisionPrimitive() const { return collisionPrimitive_; }

    /// <summary>
    /// 衝突形状を設定（球、AABB、OBBなど）
    /// </summary>
    /// <param name="collisionPrimitive">衝突形状</param>
    void SetCollisionPrimitive(uint32_t collisionPrimitive) { collisionPrimitive_ = collisionPrimitive; }

    /// <summary>
    /// コライダーの境界ボックスを設定
    /// </summary>
    void SetBounds() {
        lineBox_ = std::make_unique<LineBox>();
        if (priType == Primitive::kAABB) {
            lineBox_.reset(LineBox::Create(aabb_));
        }
        else if (priType == Primitive::kOBB) {
            lineBox_.reset(LineBox::Create(Math::ConvertOBBToAABB(obb_)));
        }
    }

    /// <summary>
    /// 衝突境界の可視化を行う
    /// </summary>
    /// <param name="world">ワールドトランスフォーム</param>
    /// <param name="camera">ビューおよびプロジェクション</param>
    void RenderCollisionBounds(WorldTransform& world, const ViewProjection& camera) {
        camera;
        world;
#ifdef USE_IMGUI
        lineBox_->Draw(world, camera, false);
#endif // DEBUG
    }

    /// <summary>
    /// 衝突の確認状態を変更（衝突している場合、色を変更）
    /// </summary>
    /// <param name="flag">衝突フラグ（1: 衝突, 0: 衝突なし）</param>
    void CheckCollision(bool flag) {
        if (priType != Primitive::kSphere) {
            if (flag == 1) {
                lineBox_->SetColor({ 1.0f,0.0f,0.0f,1.0f });  // 衝突時に赤色
            }
            else {
                lineBox_->SetColor({ 1.0f,1.0f,1.0f,1.0f });  // 衝突しない場合、白色
            }
        }
    }

private:
    float radius_ = 1.0f; ///< コライダーの半径（球形）

    AABB aabb_{ {-1.0f,-1.0f,-1.0f},{1.0f,1.0f,1.0f} }; ///< AABB（軸平行境界ボックス）

    OBB obb_{ {0.0f,0.0f,0.0f},{{1.0f,0.0f,0.0f},{0.0f,1.0f,0.0f},{0.0f,0.0f,1.0f}},{1.0f,1.0f,1.0f} }; ///< OBB（方向付き境界ボックス）

    uint32_t collisionAttribute_ = 0xffffffff; ///< 衝突属性（衝突するオブジェクトの属性）

    uint32_t collisionMask_ = 0xffffffff; ///< 衝突マスク（衝突するオブジェクトのマスク）

    uint32_t collisionPrimitive_ = 0b1; ///< 衝突形状（球、AABB、OBBなど）

    std::unique_ptr<LineBox> lineBox_; ///< 衝突境界を描画するためのラインボックス

    Primitive priType; ///< 衝突形状の種類（球、AABB、OBB）
};


