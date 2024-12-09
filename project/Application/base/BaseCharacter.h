#pragma once
#include "CollisionManager/Collider.h"
#include <CollisionManager/CollisionConfig.h>
#include <BehaviorTree/BehaviorTreeNode.h>
#include <Enemy/Enemy.h>
#include <cassert>
#include <numbers>

/**
 * @file BaseCharacter
 * @brief 味方AIの基底クラス
 */
class BaseCharacter : public Collider {
public:
    virtual ~BaseCharacter() = default;

    //各キャラクターの初期化
    virtual void Initialize(Animations* animation,std::string className);
    //各キャラクターの更新
    virtual void Update();
    //各キャラクターの描画
    virtual void Draw(const ViewProjection& camera);
    //各キャラクターの深度値のないobjの描画
    virtual void NoDepthDraw(const ViewProjection& camera);
    //各キャラクターのUI描画
    void DrawUI();

    //移動の初期化・更新
    virtual void MoveInitialize();
    virtual void MoveUpdate();

    //ジャンプの初期化・更新
    virtual void JumpInitialize();
    virtual void JumpUpdate();

    //攻撃の初期化・更新
    virtual void AttackInitialize();
    virtual void AttackUpdate();

    //スキルの初期化・更新
    virtual void UniqueInitialize() = 0;//純粋仮想関数
    virtual void UniqueUpdate() = 0;//純粋仮想関数

    //死亡の初期化・更新
    virtual void DeadInitialize();
    virtual void DeadUpdate() = 0;//純粋仮想関数

    //敵のブレス攻撃の時の行動の初期化・更新
    virtual void BreathInitialize();
    virtual void BreathUpdate();

    //味方AIを守る動きの初期化・更新
    virtual void ProtectInitialize();
    virtual void ProtectUpdate();

    //当たり判定
    virtual void OnCollision(Collider* collider) override;

    /*---------------------状態遷移関連---------------------*/
    virtual CharacterState GetState() const { return state_; }
    virtual void SetState(CharacterState newState) { state_ = newState; }

    /*---------------------Transform系---------------------*/
    const Vector3 GetWorldPosition() const override;
    const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
    WorldTransform& GetWorldTransformBody() { return worldTransformBody_; }

    /*----------------------ゲッター-----------------------*/
    //仮想関数
    virtual bool IsAttack() { return isAttack_; }
    //メンバ関数
    float GetHp() { return hp_; }
    float GetMp() { return mp_; }
    bool IsDead() { return isDead_; }
    bool GetDrawWepon() { return drawWepon_; }
    bool GetBarrier() { return barrier_; }

    /*----------------------セッター------------------------*/
    //仮想関数
    virtual void SetLight(DirectionLight directionLight) { animation_->DirectionalLightDraw(directionLight); }

    //メンバ関数
    void SetPlayerPos(Vector3 pos) { playerPos_ = pos; }
    void SetEnemy(Enemy* enemy) {enemy_ = enemy;}
    void SetOperation(bool flag) { operation_ = flag; }
    void SetViewProjection(const ViewProjection& viewProjection) { viewProjection_ = viewProjection; }
    void SetGameStart(bool flag) { gameStart_ = flag; animationNumber_ = standby;}
    void SetBattleStart(bool flag) { battleStart_ = flag;}
    void SetBarrier(bool barrier) { barrier_ = barrier; }
    void SetBarrierPos(Vector3 pos) { barrierPos_ = pos; }
    void SetHenchman(EnemyHenchman* henchman){ henchmans_.push_back(henchman); }

    // パーツ親子関係
    virtual void Relationship();

    //位置の初期化
    virtual void InitPos(float posX) { worldTransformBase_.translate = { posX,0.0f,-35.0f }; worldTransformBase_.rotate = { 0.0f,0.0f,0.0f };}

    //逃げる方向
    virtual void RunAway();

    // 共通の関数
    //プレイヤーに追従
    void followPlayer();
    //ディゾルブ
    void Dissolve();
    //敵を探す
    void searchTarget();//敵との距離感
    //敵の視野内にいるかどうか
    void IsVisibleToEnemy();
    //barrierの範囲内か
    void BarrierRange();
    

    //次の状態遷移をノードから検索
    CharacterState NextState(std::string name, int outputNum);
private:
    //クラスごとの初期化
    void InitializePerCharacter();

    //敵がどのキャラを狙っているか
    bool GetAimCharacter();

    //目的の場所までの距離
    float GetDistanceSquared(const Vector3& a,const Vector3& b);

protected:
    //状態遷移
    CharacterState state_;
    CharacterState previousState_;

    WorldTransform worldTransformBase_;//ベース
    WorldTransform worldTransformBody_;//体
    WorldTransform worldTransformShadow_;//影
    ViewProjection viewProjection_;

    /*---------------UI----------------*/
    std::unique_ptr<Sprite> spriteHP_;
    std::unique_ptr<Sprite> spriteHPG_;
    std::unique_ptr<Sprite> spriteMP_;
    std::unique_ptr<Sprite> spriteMPG_;
    std::unique_ptr<Sprite> spriteH_;
    std::unique_ptr<Sprite> spriteM_;
    std::unique_ptr<Sprite> spriteNumP_;
    std::unique_ptr<Sprite> spriteName_;
    std::unique_ptr<Sprite> HPnum_[3];
    std::unique_ptr<Sprite> MPnum_[3];

    //影のモデル
    std::unique_ptr<Model> shadowModel_;
    Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

    //ダメージ表示
    WorldTransform worldTransformNum_;
    //ダメージのモデル
    std::unique_ptr<Model> damageModel_;
    float alpha_;
    Vector3 numMove_;

    //アニメーション
    Animations* animation_;
    int animationNumber_;
    enum AnimationNumber {
        animeAttack,//攻撃
        death,//死亡
        jump,//ジャンプ
        standby,//待機
        run,//移動
    };
    float flameTime_;//フレームタイム
    float threshold_;//ディゾルブ
    // 目標の角度
    float destinationAngleY_ = 0.0f;
    //プレイヤー座標
    float minDistance_ = 8.0f;
    bool followPlayer_;
    bool drawWepon_ = false;

    // 敵を探すフラグ
    bool searchTarget_ = false;
    Vector3 randPos_;

    //ノードエディター
    Editor::NodeEditor editor_;
    //敵の情報
    Enemy* enemy_;
    float enemylength_;
    int distance_ = 1;

    float kDegreeToRandian = 3.141592f / 180.0f;

    //敵の視野内
    //最小距離
    float enemyMinDistance_ = 2.0f;
    //最大距離
    float enemyMaxDistance_ = 45.0f;
    //角度範囲
    float angleRange_ = 35.0f * kDegreeToRandian;
    //敵の攻撃範囲ないかどうか
    bool isArea_ = false;

    //ヒット確認用フラグ
    bool preHit_;
    bool isHit_;
    bool isDead_ = false;
    bool isAttack_;
    //ゲームスタートフラグ
    bool gameStart_ = false;
    //バトルが始まったかどうかのフラグ
    bool battleStart_ = false;

    float hp_ = 100.0f;
    float mp_ = 100.0f;
    //ジャンプ可能なカウント
    int jumpCount_;
    //攻撃ができるようになるまでの
    int coolTime_ = 60;
    // 速度
    Vector3 velocity_ = {};

    Vector3 playerPos_;
    //タンクのpos(タンク以外のキャラがbarrierの中に入るときに必要)
    Vector3 tankPos_;
    //レンジャーのpos(レンジャー以外のキャラがレンジャーを守るときに必要)
    Vector3 renjuPos_;

    //作戦
    bool operation_;

    //クラスの名前
    std::string className_;
    //ユニークスキルのノードの名前
    std::string skillName_;

    //ビルビード用
    Matrix4x4 backToFrontMatrix = {};

    Vector4 hpColor_ = { 1.0f,1.0f,1.0f,1.0f };//hp文字の色
    Vector4 hpNumColor_;//hp数字の色

    //barrier展開の確認
    bool barrier_ = false;
    //barrierのポジション
    Vector3 barrierPos_;
    //
    float barrierThreshold_;

    //敵子分のposを格納
    std::list<EnemyHenchman*> henchmans_;
    //レンジャーとの距離が最も近い子分のpos
    Vector3 henchmanDist_;
    int henchmanNum_;
    bool henchmanSearch_ = false;
  
};

