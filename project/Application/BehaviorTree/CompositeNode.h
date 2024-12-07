#pragma once
#include "BehaviorTreeNode.h"
#include <vector>

/**
 * @class CompositeNode
 * @brief 複数の子ノードを持つビヘイビアツリーのノード
 */
class CompositeNode : public BehaviorTreeNode {
public:
    ~CompositeNode() override;

    // 子ノードを追加する
    void AddChild(BehaviorTreeNode* child) override;

protected:
    std::vector<BehaviorTreeNode*> children;
};


