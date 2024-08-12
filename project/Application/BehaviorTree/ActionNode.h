#pragma once
#include "BehaviorTreeNode.h"

class ActionNode : public BehaviorTreeNode {
public:
    ActionNode();
    NodeStatus Update() override;

    // アクションの実行を模擬するメソッド
    virtual NodeStatus ExecuteAction() = 0;
};


