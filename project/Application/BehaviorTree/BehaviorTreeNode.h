#pragma once

enum class CharacterState {
    Attacking,
    Moveing,
    Jumping,
    Dead
};

enum class NodeStatus {
    SUCCESS,
    FAILURE,
    RUNNING
};

class BehaviorTreeNode {
public:
    virtual ~BehaviorTreeNode() = default;

    // ノードの更新を実行する純粋仮想関数
    virtual NodeStatus Update() = 0;

    // 子ノードを追加するメソッド
    virtual void AddChild(BehaviorTreeNode* child) = 0;
};


