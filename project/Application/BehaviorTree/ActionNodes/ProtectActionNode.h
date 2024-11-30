#pragma once
#include "ActionNode.h"

template<typename CharacterType>
class ProtectActionNode : public ActionNode<CharacterType> {
public:
    ProtectActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}

    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Protect) {
            this->character_->ProtectUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }

    void Initialize() override {
        // ProtectActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Protect) {
            this->character_->ProtectInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        child;
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};