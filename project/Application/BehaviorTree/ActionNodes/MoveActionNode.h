#pragma once
#include "ActionNode.h"

template<typename CharacterType>
class MoveActionNode : public ActionNode<CharacterType> {
public:
    MoveActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}

    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Moveing) {
            this->character_->MoveUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }

    void AddChild(BehaviorTreeNode* child) override {
        // MoveActionNode では子ノードを持たないので、このメソッドは空実装
    }
};