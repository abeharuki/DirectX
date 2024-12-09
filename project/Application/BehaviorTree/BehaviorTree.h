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
#include "ActionNodes/UniqueActionNode.h"
#include "ActionNodes/BreathActionNode.h"
#include "ActionNodes/ProtectActionNode.h"

/**
 * @file BehaviorTree
 * @brief キャラクターの行動を管理するビヘイビアツリークラス
 */
template <typename CharacterType>
class BehaviorTree {
public:
    BehaviorTree(CharacterType* character);
    void Initialize();//BehaviorTreeの初期化
    void NodeInitialize();//各ノードの初期化
    void Update();//ルートノードから更新を実行し、各アクションを評価・実行

    // ノードを取得する
    std::vector<BehaviorTreeNode*> GetNodes() const;
    //ルートノードの取得する
    SelectorNode* GetRootNode() const;

private:
    CharacterType* character_;  //対象となるキャラクター
    SelectorNode* rootNode_; //ルートノード
    std::vector<BehaviorTreeNode*> nodes_;  // ノードのコレクション
};

//ビヘイビアツリー初期化の呼び出し
template <typename CharacterType>
BehaviorTree<CharacterType>::BehaviorTree(CharacterType* character) : character_(character), rootNode_(nullptr) {
    Initialize();
}

//ビヘイビアツリーの初期化
template <typename CharacterType>
void BehaviorTree<CharacterType>::Initialize() {
    auto* moveActionNode = new MoveActionNode<CharacterType>(character_);
    auto* jumpActionNode = new JumpActionNode<CharacterType>(character_);
    auto* attackActionNode = new AttackActionNode<CharacterType>(character_);
    auto* deadActionNode = new DeadActionNode<CharacterType>(character_);
    auto* uniqueActionNode = new UniqueActionNode<CharacterType>(character_);
    auto* breathActionNode = new BreathActionNode<CharacterType>(character_);
    auto* protectActionNode = new ProtectActionNode<CharacterType>(character_);

    rootNode_ = new SelectorNode();
    rootNode_->AddChild(moveActionNode);
    rootNode_->AddChild(jumpActionNode);
    rootNode_->AddChild(attackActionNode);
    rootNode_->AddChild(deadActionNode);
    rootNode_->AddChild(uniqueActionNode);
    rootNode_->AddChild(breathActionNode);
    rootNode_->AddChild(protectActionNode);

    // ノードコレクションにノードを追加
    nodes_.push_back(moveActionNode);
    nodes_.push_back(jumpActionNode);
    nodes_.push_back(attackActionNode);
    nodes_.push_back(deadActionNode);
    nodes_.push_back(uniqueActionNode);
    nodes_.push_back(breathActionNode);
    nodes_.push_back(protectActionNode);
    nodes_.push_back(rootNode_);

}

//ノードの初期化
template <typename CharacterType>
void BehaviorTree<CharacterType>::NodeInitialize() {
    if (rootNode_) {
        rootNode_->Initialize();
    }
}

//ノードの更新
template <typename CharacterType>
void BehaviorTree<CharacterType>::Update() {
    if (rootNode_) {
        rootNode_->Update();
    }
}

//ノードを取得
template <typename CharacterType>
std::vector<BehaviorTreeNode*> BehaviorTree<CharacterType>::GetNodes() const {
    return nodes_;
}

//ルートノードの取得
template <typename CharacterType>
SelectorNode* BehaviorTree<CharacterType>::GetRootNode() const {
    return rootNode_;
}