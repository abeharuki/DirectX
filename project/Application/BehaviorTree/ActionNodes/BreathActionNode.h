#pragma once
#include "ActionNode.h"

/**
 * @class BreathActionNode
 * @brief キャラクターの敵がブレス攻撃をしてきたときのアクションを処理するビヘイビアツリーのアクションノード
 */
template<typename CharacterType>
class BreathActionNode : public ActionNode<CharacterType> {
public:
    BreathActionNode(CharacterType* character) : ActionNode<CharacterType>(character) {}

    NodeStatus Update() override {
        if (this->character_->GetState() == CharacterState::Breath) {
            this->character_->BreathUpdate();
            return NodeStatus::SUCCESS;
        }
        return NodeStatus::FAILURE;
    }

    void Initialize() override {
        // BreathActionNode の初期化処理を実装
        if (this->character_->GetState() == CharacterState::Breath) {
            this->character_->BreathInitialize();
        }
    }

    void AddChild(BehaviorTreeNode* child) override {
        child;
        // ActionNode では子ノードを持たないので、このメソッドは空実装
    }
};