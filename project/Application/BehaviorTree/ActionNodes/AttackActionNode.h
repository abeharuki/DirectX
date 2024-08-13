#pragma once
#include "ActionNode.h"

template<typename CharacterType>
class AttackActionNode : public ActionNode<CharacterType> {
public:
    AttackActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}
    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Attacking) {
            this->character_->AttackUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;  // 状態が合わない場合は失敗
    }
    void AddChild(BehaviorTreeNode* child) override {
        // MoveActionNode では子ノードを持たないので、このメソッドは空実装
    }
};