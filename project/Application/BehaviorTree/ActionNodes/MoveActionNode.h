#pragma once
#include "ActionNode.h"

/**
 * @class MoveActionNode
 * @brief キャラクターの行動アクションを処理するビヘイビアツリーのアクションノード
 */
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

    void Initialize() override {
        // MoveActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Moveing) {
            this->character_->MoveInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        child;
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};