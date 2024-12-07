#pragma once

/**
 * @enum CharacterState
 * @brief キャラクターの状態を表す列挙型
 */
enum class CharacterState {
    Attacking,
    Moveing,
    Jumping,
    Unique,
    Breath,
    Protect,
    Dead
};

/**
 * @enum NodeStatus
 * @brief ビヘイビアツリーのノードの実行状態を表す列挙型
 */
enum class NodeStatus {
    SUCCESS,
    FAILURE,
    RUNNING
};

/**
* @class BehaviorTreeNode.h
* @brief ビヘイビアツリーのノードを表す基底クラス
*/
class BehaviorTreeNode {
public:
    virtual ~BehaviorTreeNode() = default;

    //ノードの初期化
    virtual void Initialize() = 0;

    // ノードの更新
    virtual NodeStatus Update() = 0;

    // 子ノードを追加
    virtual void AddChild(BehaviorTreeNode* child) = 0;
};


