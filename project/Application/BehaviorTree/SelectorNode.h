#pragma once
#include "CompositeNode.h"

//orのクラスどれか一つが実行で来たらseuccessを返す
class SelectorNode : public CompositeNode {
public:
    void Initialize() override;
    NodeStatus Update() override;
};


