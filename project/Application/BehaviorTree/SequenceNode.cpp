#include "SequenceNode.h"

NodeStatus SequenceNode::Update() {
    for (BehaviorTreeNode* child : children) {
        NodeStatus status = child->Update();
        if (status == NodeStatus::FAILURE) {
            return NodeStatus::FAILURE;
        }
        if (status == NodeStatus::RUNNING) {
            return NodeStatus::RUNNING;
        }
    }
    return NodeStatus::SUCCESS;
}
