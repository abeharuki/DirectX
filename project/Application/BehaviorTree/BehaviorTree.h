#pragma once
#include "BehaviorTreeNode.h"

class BehaviorTree {
public:
    BehaviorTree(BehaviorTreeNode* rootNode);
    ~BehaviorTree();

    // ビヘイビアツリーの更新を行う
    void Update();

private:
    BehaviorTreeNode* root;
};

