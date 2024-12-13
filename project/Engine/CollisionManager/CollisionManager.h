#pragma once
#include "Collider.h"
#include <list>

/**
 * @class CollisionManager
 * @brief 衝突判定を管理するクラス
 */

class CollisionManager
{
public:
    /// <summary>
    /// コライダーリストをクリアする
    /// </summary>
    void ClearColliderList();

    /// <summary>
    /// コライダーをリストに追加する
    /// </summary>
    /// <param name="collider">追加するコライダー</param>
    void SetColliderList(Collider* collider);

    /// <summary>
    /// リスト内の全てのコライダー同士の衝突をチェックする
    /// </summary>
    void CheckAllCollisions();

private:
    /// <summary>
    /// 2つのコライダー同士の衝突をチェックする
    /// </summary>
    /// <param name="colliderA">最初のコライダー</param>
    /// <param name="colliderB">2番目のコライダー</param>
    void CheckCollisionPair(Collider* colliderA, Collider* colliderB);

    /// <summary>
    /// 球と球の衝突判定
    /// </summary>
    /// <param name="sphereA">最初の球</param>
    /// <param name="sphereB">2番目の球</param>
    /// <returns>衝突している場合は true、それ以外は false</returns>
    bool CheckCollisionSphere(const sphere& sphereA, const sphere& sphereB);

    /// <summary>
    /// 球とAABB（軸平行境界ボックス）の衝突判定
    /// </summary>
    /// <param name="sphere">球</param>
    /// <param name="aabb">AABB</param>
    /// <returns>衝突している場合は true、それ以外は false</returns>
    bool CheckCollisionSphereAABB(const sphere& sphere, const AABB& aabb);

    /// <summary>
    /// AABB同士の衝突判定
    /// </summary>
    /// <param name="aabbA">最初のAABB</param>
    /// <param name="aabbB">2番目のAABB</param>
    /// <returns>衝突している場合は true、それ以外は false</returns>
    bool CheckCollisionAABB(const AABB& aabbA, const AABB& aabbB);

    /// <summary>
    /// AABBとOBB（軸平行境界ボックスとオリエンテッドボックス）の衝突判定
    /// </summary>
    /// <param name="aabb">AABB</param>
    /// <param name="obb">OBB</param>
    /// <returns>衝突している場合は true、それ以外は false</returns>
    bool CheckCollisionAABBOBB(const AABB& aabb, const OBB& obb);

    /// <summary>
    /// OBB同士の衝突判定
    /// </summary>
    /// <param name="obbA">最初のOBB</param>
    /// <param name="obbB">2番目のOBB</param>
    /// <returns>衝突している場合は true、それ以外は false</returns>
    bool CheckCollisionOBB(const OBB& obbA, const OBB& obbB);

private:
    std::list<Collider*> colliders_{}; ///< 衝突をチェックするコライダーのリスト
};


