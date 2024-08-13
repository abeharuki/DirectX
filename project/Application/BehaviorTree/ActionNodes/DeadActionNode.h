#pragma once
#include "ActionNode.h"

template<typename CharacterType>
class DeadActionNode : public ActionNode<CharacterType> {
public:
    DeadActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}

    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Dead) {
            this->character_->DeadUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }

    void AddChild(BehaviorTreeNode* child) override {
        // MoveActionNode では子ノードを持たないので、このメソッドは空実装
    }
};