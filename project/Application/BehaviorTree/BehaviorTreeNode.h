#pragma once

enum class CharacterState {
    Attacking,
    Moveing,
    Jumping,
    Unique,
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

    //ノードの初期化
    virtual void Initialize() = 0;

    // ノードの更新
    virtual NodeStatus Update() = 0;

    // 子ノードを追加
    virtual void AddChild(BehaviorTreeNode* child) = 0;
};


