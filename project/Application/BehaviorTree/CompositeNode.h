#pragma once
#include "BehaviorTreeNode.h"
#include <vector>

class CompositeNode : public BehaviorTreeNode {
public:
    ~CompositeNode() override;

    // 子ノードを追加する
    void AddChild(BehaviorTreeNode* child) override;

protected:
    std::vector<BehaviorTreeNode*> children;
};


