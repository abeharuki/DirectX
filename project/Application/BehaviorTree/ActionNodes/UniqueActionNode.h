#pragma once
#include "ActionNode.h"

/**
 * @class UniqeActionNode
 * @brief キャラクターのスキルアクションを処理するビヘイビアツリーのアクションノード
 */
template<typename CharacterType>
class UniqueActionNode : public ActionNode<CharacterType> {
public:
    UniqueActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}

    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Unique) {
            this->character_->UniqueUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }

    void Initialize() override {
        // MoveActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Unique) {
            this->character_->UniqueInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        child;
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};