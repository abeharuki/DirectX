#pragma once
#include "../BehaviorTreeNode.h"

template<typename CharacterType>
class ActionNode : public BehaviorTreeNode {
public:
    ActionNode(CharacterType* character) : character_(character) {}

    // ActionNode クラスでは特定のアクションを実行するため、Update メソッドは派生クラスで実装
    NodeStatus Update() override = 0;


protected:
    CharacterType* character_;
};