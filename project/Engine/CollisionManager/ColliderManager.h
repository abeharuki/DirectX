#pragma once
#include "CollisionManager/Collider.h"


/**
 * @class ColliderManager
 * @brief 複数のコライダーの管理と衝突判定を行うクラス
 */
class ColliderManager : public Collider {
public:

    /// <summary>
    /// 管理するコライダーの半径を設定（球形コライダー）
    /// </summary>
    /// <param name="radius">半径</param>
    void SetManagerRadius(float radius) {
        SetRadius(radius);
        priType = Primitive::kSphere; // 球形コライダーとして設定
    };

    /// <summary>
    /// 管理するコライダーのAABB（軸平行境界ボックス）を設定
    /// </summary>
    /// <param name="aabb">AABB</param>
    void SetManagerAABB(AABB aabb) {
        SetAABB(aabb);
        priType = Primitive::kAABB; // AABBコライダーとして設定
    }

    /// <summary>
    /// 衝突属性を設定
    /// </summary>
    /// <param name="collisionAttribute">衝突属性</param>
    void SetAttribute(uint32_t collisionAttribute) { SetCollisionAttribute(collisionAttribute); };

    /// <summary>
    /// 衝突マスクを設定
    /// </summary>
    /// <param name="collisionMask">衝突マスク</param>
    void SetMask(uint32_t collisionMask) { SetCollisionMask(collisionMask); };

    /// <summary>
    /// 衝突形状を設定（球、AABB、OBBなど）
    /// </summary>
    /// <param name="collisionPrimitive">衝突形状</param>
    void SetPrimitive(uint32_t collisionPrimitive) { SetCollisionPrimitive(collisionPrimitive); };

    /// <summary>
    /// コライダーの可視化を行う
    /// </summary>
    /// <param name="viewProjection">ビューおよびプロジェクション</param>
    void Draw(const ViewProjection& viewProjection);

    /// <summary>
    /// コライダーのワールドトランスフォームを設定（仮置き）
    /// </summary>
    /// <param name="worldTransform">ワールドトランスフォーム</param>
    void SetWorldTransform(WorldTransform worldTransform) { worldTransform_ = worldTransform; }

    /// <summary>
    /// 衝突処理
    /// </summary>
    /// <param name="collider">衝突相手のコライダー</param>
    void OnCollision(Collider* collider) override;

    /// <summary>
    /// コライダーのワールド座標を取得
    /// </summary>
    /// <returns>ワールド座標</returns>
    const Vector3 GetWorldPosition() const override;

    /// <summary>
    /// コライダーのワールドトランスフォームを取得
    /// </summary>
    /// <returns>ワールドトランスフォーム</returns>
    const WorldTransform& GetWorldTransform() const override { return worldTransform_; }

    /// <summary>
    /// 衝突の確認状態を変更（衝突フラグに応じて境界ボックスの色を変更）
    /// </summary>
    /// <param name="flag">衝突フラグ（1: 衝突, 0: 衝突なし）</param>
    void Chack(bool flag) { CheckCollision(flag); }

private:
    WorldTransform worldTransform_; ///< コライダーのワールドトランスフォーム

    Primitive priType; ///< コライダーの形状タイプ（球、AABB、OBBなど）
};
