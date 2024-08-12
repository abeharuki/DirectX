#include "ActionNode.h"

ActionNode::ActionNode() = default;

NodeStatus ActionNode::Update() {
    return ExecuteAction();
}
