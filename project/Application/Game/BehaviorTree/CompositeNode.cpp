#include "CompositeNode.h"

CompositeNode::~CompositeNode() {
    for (BehaviorTreeNode* child : children) {
        delete child;
    }
}

void CompositeNode::AddChild(BehaviorTreeNode* child) {
    children.push_back(child);
}
