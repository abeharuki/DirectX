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

    void Initialize() override {
        // AttackActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Attacking) {
            this->character_->AttackInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        child;
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};