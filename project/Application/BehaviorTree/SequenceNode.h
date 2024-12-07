#pragma once
#include "CompositeNode.h"

/**
 * @class SequenceNode
 * @brief シーケンスノード（順次実行ノード）
 */
//andのクラスすべて実行できるまでFalseを返す
class SequenceNode : public CompositeNode {
public:
    NodeStatus Update() override;
};

