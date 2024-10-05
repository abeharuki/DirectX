#pragma once
#include "ActionNode.h"

template<typename CharacterType>
class JumpActionNode : public ActionNode<CharacterType> {
public:
    JumpActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}
    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Jumping) {
            this->character_->JumpUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;  // 状態が合わない場合は失敗
    }

    void Initialize() override {
        // JumpActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Jumping) {
            this->character_->JumpInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};