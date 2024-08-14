#pragma once
#include "../BehaviorTreeNode.h"

template<typename CharacterType>
class ActionNode : public BehaviorTreeNode {
public:
    ActionNode(CharacterType* character) : character_(character) {}

    void Initialize() override = 0;

    NodeStatus Update() override = 0;
  
protected:
    CharacterType* character_;
};