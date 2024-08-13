#pragma once
#include "ActionNode.h"

template<typename CharacterType>
class JumpActionNode : public ActionNode<CharacterType> {
public:
    JumpActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}
    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Moveing) {
            this->character_->JumpUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;  // 状態が合わない場合は失敗
    }


    void AddChild(BehaviorTreeNode* child) override {
        // MoveActionNode では子ノードを持たないので、このメソッドは空実装
    }
};