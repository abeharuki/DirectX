#include "BehaviorTree.h"

BehaviorTree::BehaviorTree(BehaviorTreeNode* rootNode) : root(rootNode) {}

BehaviorTree::~BehaviorTree() {
    delete root;
}

void BehaviorTree::Update() {
    if (root) {
        root->Update();
    }
}
