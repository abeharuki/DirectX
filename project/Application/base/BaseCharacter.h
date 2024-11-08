#pragma once
#include "Model.h"
#include "CollisionManager/Collider.h"
#include <BehaviorTree/BehaviorTreeNode.h>
#include <Enemy/Enemy.h>

class BaseCharacter : public Collider {
public:
    virtual ~BaseCharacter() = default;

    virtual void Initialize(const Animation* animation) = 0;
    virtual void Update() = 0;
    virtual void Draw(const ViewProjection& camera) = 0;
    virtual void NoDepthDraw(const ViewProjection& camera) = 0;

    virtual void MoveInitialize() = 0;
    virtual void MoveUpdate() = 0;
    virtual void JumpInitialize() = 0;
    virtual void JumpUpdate() = 0;

    virtual void AttackInitialize() = 0;
    virtual void AttackUpdate() = 0;

    virtual void UniqueInitialize() = 0;
    virtual void UniqueUpdate() = 0;

    virtual void DeadInitialize() = 0;
    virtual void DeadUpdate() = 0;

    virtual void OnCollision(Collider* collider) override = 0;

    /*---------------------状態遷移関連---------------------*/
    virtual CharacterState GetState() const { return state_; }
    virtual void SetState(CharacterState newState) { state_ = newState; }

    /*---------------------Transform系---------------------*/
    const Vector3 GetWorldPosition() const override;
    const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
    WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }

    /*----------------------ゲッター-----------------------*/
    //純粋仮想関数
    virtual bool IsAttack() = 0;
    //メンバ関数
    float GetHp() { return hp_; }
    float GetMp() { return mp_; }
    bool IsDead() { return isDead_; }

    /*----------------------セッター------------------------*/
    //仮想関数
    virtual void SetLight(DirectionLight directionLight);

    //メンバ関数
    void SetEnemy(Enemy* enemy) {enemy_ = enemy;}
    void SetOperation(bool flag) { operation_ = flag; }
    void SetViewProjection(const ViewProjection& viewProjection) { viewProjection_ = viewProjection; }


    //プレイヤーに追従
    virtual void followPlayer(Vector3 playerPos) = 0;
    // パーツ親子関係
    virtual void Relationship() = 0;

    //位置の初期化
    virtual void InitPos();//引数に座標をいれられるようにする

    // 共通の関数
    //ディゾルブ
    void Dissolve();
    //敵を探す
    void searchTarget(Vector3 enemyPos);
    //敵の視野内にいるかどうか
    void IsVisibleToEnemy();
    //逃げる方向
    void RunAway();
    //次の状態遷移をノードから検索
    CharacterState NextState(std::string name, int outputNum);

    
protected:
    //状態遷移
    CharacterState state_;
    CharacterState previousState_;

    WorldTransform worldTransformBase_;
    WorldTransform worldTransformBody_;
    ViewProjection viewProjection_;

    Animation* animation_;

    float hp_ = 100.0f;
    float mp_ = 100.0f;
    bool isDead_ = false;

    //作戦
    bool operation_;

    //敵の情報
    Enemy* enemy_;  
};

