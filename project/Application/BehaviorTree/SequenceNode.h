#pragma once
#include "CompositeNode.h"

//andのクラスすべて実行できるまでFalseを返す
class SequenceNode : public CompositeNode {
public:
    NodeStatus Update() override;
};

