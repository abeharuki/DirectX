#pragma once
#include "BehaviorTreeNode.h"

class ConditionNode : public BehaviorTreeNode {
public:
    ConditionNode();
    NodeStatus Update() override;

    // 条件を評価するメソッド
    virtual bool CheckCondition() const = 0;
};
