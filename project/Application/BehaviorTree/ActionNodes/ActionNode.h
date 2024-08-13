#pragma once
#include "../BehaviorTreeNode.h"

template<typename CharacterType>
class ActionNode : public BehaviorTreeNode {
public:
    ActionNode(CharacterType* character) : character_(character) {}

    // ActionNode クラスでは特定のアクションを実行するため、Update メソッドは派生クラスで実装
    NodeStatus Update() override = 0;
    virtual void Initialize() = 0;  // 初期化メソッドを純粋仮想関数として定義


protected:
    CharacterType* character_;
};