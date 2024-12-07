#pragma once
#include "BehaviorTreeNode.h"

/**
 * @class ConditionNode.h
 * @brief 条件を評価するビヘイビアツリーのノード
 */
class ConditionNode : public BehaviorTreeNode {
public:
    ConditionNode();
    NodeStatus Update() override;

    // 条件を評価するメソッド
    virtual bool CheckCondition() const = 0;
};

