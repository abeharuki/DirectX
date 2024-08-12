#pragma once
#include "CompositeNode.h"

class SequenceNode : public CompositeNode {
public:
    NodeStatus Update() override;
};

