#include "SelectorNode.h"

NodeStatus SelectorNode::Update() {
    for (BehaviorTreeNode* child : children) {
        NodeStatus status = child->Update();
        if (status == NodeStatus::SUCCESS) {
            return NodeStatus::SUCCESS;
        }
        if (status == NodeStatus::RUNNING) {
            return NodeStatus::RUNNING;
        }
    }
    return NodeStatus::FAILURE;
}
