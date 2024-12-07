#pragma once
#include "CompositeNode.h"

/**
 * @class ConditionNode
 * @brief セレクターノード（選択ノード）
 */
//orのクラスどれか一つが実行で来たらseuccessを返す
class SelectorNode : public CompositeNode {
public:
    void Initialize() override;
    NodeStatus Update() override;
};


