#pragma once
#include "ActionNode.h"

/**
 * @class DeadActionNode.h
 * @brief キャラクターの死亡アクションを処理するビヘイビアツリーのアクションノード
 */
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

    void Initialize() override {
        // DeadActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Dead) {
            this->character_->DeadInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        child;
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};