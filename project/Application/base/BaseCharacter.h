#pragma once
#include "Model.h"
#include "CollisionManager/Collider.h"
#include <BehaviorTree/BehaviorTreeNode.h>

class BaseCharacter : public Collider {
public:
    virtual ~BaseCharacter() = default;

    virtual void Initialize() = 0;
    virtual void Update() = 0;
    virtual void Draw(const ViewProjection& camera) = 0;
    virtual void NoDepthDraw(const ViewProjection& camera) = 0;

    virtual void SetState(CharacterState newState) = 0;
    virtual CharacterState GetState() const = 0;
    virtual void OnCollision(Collider* collider) override = 0;

    // 共通のゲッターメソッド
    virtual float GetHp() const = 0;
    virtual bool IsDead() const = 0;

protected:



};
