#include "ConditionNode.h"

ConditionNode::ConditionNode() = default;

NodeStatus ConditionNode::Update() {
    return CheckCondition() ? NodeStatus::SUCCESS : NodeStatus::FAILURE;
}
