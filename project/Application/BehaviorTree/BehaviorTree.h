#pragma once
#include "BehaviorTreeNode.h"
#include "CompositeNode.h"
#include "SelectorNode.h"
#include "SequenceNode.h"
#include "ActionNodes/ActionNode.h"
#include "ActionNodes/JumpActionNode.h"
#include "ActionNodes/MoveActionNode.h"
#include "ActionNodes/AttackActionNode.h"
#include "ActionNodes/DeadActionNode.h"

template <typename CharacterType>
class BehaviorTree {
public:
    BehaviorTree(CharacterType* character);
    void Initialize();
    void Update();

private:
    CharacterType* character_;
    SelectorNode* rootNode_;
};

template <typename CharacterType>
BehaviorTree<CharacterType>::BehaviorTree(CharacterType* character) : character_(character), rootNode_(nullptr) {
    Initialize();
}

template <typename CharacterType>
void BehaviorTree<CharacterType>::Initialize() {
    auto* moveActionNode = new MoveActionNode<CharacterType>(character_);
    auto* jumpActionNode = new JumpActionNode<CharacterType>(character_);
    auto* attackActionNode = new AttackActionNode<CharacterType>(character_);
    auto* deadActionNode = new DeadActionNode<CharacterType>(character_);

    rootNode_ = new SelectorNode();
    rootNode_->AddChild(moveActionNode);
    rootNode_->AddChild(jumpActionNode);
    rootNode_->AddChild(attackActionNode);
    rootNode_->AddChild(deadActionNode);
}

template <typename CharacterType>
void BehaviorTree<CharacterType>::Update() {
    if (rootNode_) {
        rootNode_->Update();
    }
}