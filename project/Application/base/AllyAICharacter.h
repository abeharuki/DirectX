#pragma once
#include "CollisionManager/Collider.h"
#include <CollisionManager/CollisionConfig.h>
#include <BehaviorTree/BehaviorTreeNode.h>
#include <Enemy/Enemy.h>
#include <cassert>
#include <numbers>
#include <Sprite.h>

//味方AIキャラクターの定数
namespace AllyAIConstants {

    //共通サイズ関連
    const Vector2 kSpriteHPSize = { 100.0f, 10.0f };//HPバーのサイズ
    const Vector2 kSpriteMPSize = { 100.0f, 10.0f };//MPバーのサイズ
    const Vector2 kSpriteHSize = { 35.0f, 35.0f };//Hスプライトのサイズ
    const Vector2 kSpriteMSize = { 35.0f, 35.0f };//Mスプライトのサイズ
    const Vector2 kSpriteNumSize = { 36.0f, 36.0f };//hp・mpのサイズ
    const Vector2 kSpriteNumPSize = { 93.0f, 85.0f };//何Pか描画するスプライトのサイズ
    const Vector2 kSpriteNameSize = { 106.0f, 50.0f };//キャラクターネームスプライトのサイズ

    // 影関連
    const float kShadowAlphaAdjustment = 3.9f;
    const Vector3 kShadowRotation = { -1.571f,0.0f,0.0f };//影の角度
    const Vector3 kShadowScaleTank = { 2.2f,2.2f,1.0f };//タンクの影サイズ
    const Vector3 kShadowScaleDefault = { 1.8f,1.8f,1.0f };//タンク以外の影サイス
    const float kShadowTranslateOffset = 0.1f;// 影のオフセット

    //フレーム関連
    const float kFlameTimeInit = 30.0f;//フレームタイムの初期値

    // ダメージ関連
    const Vector3 kWorldTransformNumScale = { 0.5f, 0.5f, 0.5f };//ダメージのサイズ
    const float kDamageInitAlpha = 0.0f;//ダメージスプライトの初期アルファ値
    const float kDamageAlphaInitValue = 2.0f;  // プレイヤーが食らったときダメージ表示の初期アルファ値（透明度）
    const float kDamageAlphaDecrement = 0.08f;  // ダメージ表示のアルファ値の減少速度
    const float kDamageDisplayHeight = 2.0f;  // ダメージ表示のY座標オフセット
    const float kEnemyDamageNormal = 10.0f;  // 通常攻撃,投擲、ダッシュのダメージ値
    const float kEnemyDamageBreath = 20.0f;  // ブレス攻撃,地面を殴るのダメージ値
    const float kEnemyDamageHenchman = 50.0f;  // 子分攻撃のダメージ値
    
    // HP/MP関連
    const int kMaxHPMPDisplay = 100;//HP・MPの最大値
    const float kLowHPThreshold = 20.0f;//HP・MPの閾値
    const float kMediumHPThreshold = 50.0f;//HP・MPの閾値

    // 色
    const Vector4 kLowHPColor = { 5.0f, 0.0f, 0.0f, 1.0f };//lowHPの時の色
    const Vector4 kMediumHPColor = { 1.0f, 0.2f, 0.0f, 1.0f };//HPが半分以下の時の色
    const Vector4 kDefaultHPColor = { 1.0f, 1.0f, 1.0f, 1.0f };//HP文字・数字のデフォルト色

    const float kCollisionPushOutFactor = 0.3f; // AABB-OBBの押し出し係数

    /*-----------------------各キャラクターのUI座標初期化-------------------------*/

    // 共通の位置
    const float kSpriteBarXPos = 1106.0f;//バー関連のX座標
    const float kSpriteIconXPos = 1097.0f;//HP・MPアイコンのX座標
    const float kSpriteIdentifyXPos = 995.0f;//識別スプライト(名前と何Pか表すスプライト)のX座標
    const float kNumXPos = 1172.0f;//数字スプライトのX座標
    const float kNumXOffset = 16.0f;//数字スプライトのX座標をを桁ごとにずらすためのオフセット


    //ヒーラーの各UI位置
    const float kHealerSpriteHPBarYPos = 615.0f;//HPバー関連のY座標
    const float kHealerSpriteMPBarYPos = 640.0f;//MPバー関連のY座標
    const float kHealerSpriteNumPYPos = 583.0f;//識別スプライト(何Pか表すスプライト)のY座標
    const float kHealerSpriteHIconYPos = 593.0f;//HPアイコンのY座標
    const float kHealerSpriteMIconYPos = 618.0f;//MPアイコンのY座標
    const float kHealerSpriteNameYPos = 573.0f;//識別スプライト(名前)のY座標
    const float kHealerHPNumYPos = 595.0f;//HP数字スプライトのY座標
    const float kHealerMPNumYPos = 620.0f;//MP数字スプライトのY座標
    
    //レンジャーの各UI位置
    const float kRenjuSpriteHPBarYPos = 545.0f;//HPバー関連のY座標
    const float kRenjuSpriteMPBarYPos = 570.0f;//MPバー関連のY座標
    const float kRenjuSpriteNumPYPos = 513.0f;//識別スプライト(何Pか表すスプライト)のY座標
    const float kRenjuSpriteHIconYPos = 523.0f;//HPアイコンのY座標
    const float kRenjuSpriteMIconYPos = 548.0f;//MPアイコンのY座標
    const float kRenjuSpriteNameYPos = 503.0f;//識別スプライト(名前)のY座標
    const float kRenjuHPNumYPos = 525.0f;//HP数字スプライトのY座標
    const float kRenjuMPNumYPos = 560.0f;//MP数字スプライトのY座標

    //タンクの各UI位置
    const float kTankSpriteHPBarYPos = 475.0f;//HPバー関連のY座標
    const float kTankSpriteMPBarYPos = 500.0f;//MPバー関連のY座標
    const float kTankSpriteNumPYPos = 443.0f;//識別スプライト(何Pか表すスプライト)のY座標
    const float kTankSpriteHIconYPos = 453.0f;//HPアイコンのY座標
    const float kTankSpriteMIconYPos = 478.0f;//MPアイコンのY座標
    const float kTankSpriteNameYPos = 433.0f;//識別スプライト(名前)のY座標
    const float kTankHPNumYPos = 455.0f;//HP数字スプライトのY座標
    const float kTankMPNumYPos = 480.0f;//MP数字スプライトのY座標


    /*-----------------------各State関連-------------------------*/
     // キャラクター速度
    const float kCharacterSpeed = 0.3f;

    // 敵距離とジャンプ設定
    const float kJumpDistanceNear = 5.0f;//距離5の閾値
    const float kJumpDistanceMidLow = 10.0f;//距離10の閾値
    const float kJumpDistanceMidHigh = 20.0f;//距離20の閾値
    const float kJumpDistanceFar = 30.0f;//距離30の閾値

    // 敵攻撃影響範囲設定
    const float kImpactSizeSmall = 10.0f;//衝撃波のサイズ10閾値
    const float kImpactSizeMediumLow = 20.0f;//衝撃波のサイズ20閾値
    const float kImpactSizeMediumHigh = 40.0f;//衝撃波のサイズ40閾値
    const float kImpactSizeLarge = 60.0f;//衝撃波のサイズ60閾値

    // 敵の範囲攻撃距離
    const float kGroundAttackDistance = 35.0f;//敵の攻撃範囲

    // 弓キャラの条件
    const size_t kHenchmanCountThreshold = 1;//必殺技に遷移するときの閾値

    // 敵から遠ざかる移動倍率
    const float kEscapeFactor = 0.3f;//敵のBreath攻撃時

    // 到達判定の許容範囲
    const float kCloseDistance = 2.0f;

    // バリアしきい値
    const float kBarrierThreshold = 0.1f;

    //敵の視野内
   //最小距離
    const float kEnemyMinDistance_ = 2.0f;
    //最大距離
    const float kEnemyMaxDistance_ = 45.0f;
    //角度範囲
    const float kDegreeToRandian = 3.141592f / 180.0f;
    const float kAngleRange_ = 35.0f * kDegreeToRandian;

    // 逃げる際の速度
    const float kVelocityX = 1.0f;//横方向のvelo
    const float kVelocityZ = -1.5f;//対象から反対方向に逃げる
}


/**
 * @file AllyAICharacter
 * @brief 味方AIの基底クラス
 */
class AllyAICharacter : public Collider {
public:
    virtual ~AllyAICharacter() = default;

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
    void SetGameStart(bool flag) { gameStart_ = flag; animationNumber_ = kStandby;}
    void SetBattleStart(bool flag) { battleStart_ = flag;}
    void SetBarrier(bool barrier) { barrier_ = barrier; }
    void SetBarrierPos(Vector3 pos) { barrierPos_ = pos; }
    void SetHenchman(EnemyHenchman* henchman){ henchmans_.push_back(henchman); }

    // パーツ親子関係
    virtual void Relationship();

    //位置の初期化
    virtual void InitPos(float posX) { 
        worldTransformBase_.translate = { posX,0.0f,-35.0f };
        worldTransformBase_.rotate = { 0.0f,0.0f,0.0f };
        destinationAngleY_ = 0.0f;
    }

    //逃げる方向
    virtual void RunAway();

    // 共通の関数
    //プレイヤーに追従
    void followPlayer();
    //敵を探す
    void searchTarget();//敵との距離感
    //敵の視野内にいるかどうか
    void IsVisibleToEnemy();
    //barrierの範囲内か
    void BarrierRange();
    //目標までの回転
    void  DestinationAngle(Vector3 sub);
    //ダメージを食らったときのスプライトの座標やアルファ値の初期化
    void DameageInit() {
        alpha_ = AllyAIConstants::kDamageAlphaInitValue;
        worldTransformNum_.translate = { worldTransformBase_.translate.x,worldTransformBase_.translate.y + AllyAIConstants::kDamageDisplayHeight,worldTransformBase_.translate.z };
        numMove_ = { worldTransformNum_.translate.x ,worldTransformNum_.translate.y + AllyAIConstants::kDamageDisplayHeight,worldTransformNum_.translate.z };
    }

    //次の状態遷移をノードから検索
    CharacterState NextState(std::string name, int outputNum);

   
private:
    //クラスごとの初期化
    void InitializePerCharacter();

    //敵がどのキャラを狙っているか
    bool GetAimCharacter();

protected:
    //状態遷移
    CharacterState state_;
    CharacterState previousState_;

    WorldTransform worldTransformBase_;//ベース
    WorldTransform worldTransformBody_;//体
    WorldTransform worldTransformShadow_;//影
    ViewProjection viewProjection_;

    /*---------------UI----------------*/
    std::unique_ptr<Sprite> spriteHP_;//HPバー
    std::unique_ptr<Sprite> spriteHPG_;//HPバー背景
    std::unique_ptr<Sprite> spriteMP_;//MPバー
    std::unique_ptr<Sprite> spriteMPG_;//MPバー背景
    std::unique_ptr<Sprite> spriteH_;//Hスプライト
    std::unique_ptr<Sprite> spriteM_;//Mスプライト
    std::unique_ptr<Sprite> spriteNumP_;//何Pか表すスプライト
    std::unique_ptr<Sprite> spriteName_;//ネームスプライト
    std::unique_ptr<Sprite> HPnum_[3];//HP数値スプライト一桁目から三桁目まで
    std::unique_ptr<Sprite> MPnum_[3];//MP数値スプライト一桁目から三桁目まで

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
        kAnimeAttack,//攻撃
        kDeath,//死亡
        kJump,//ジャンプ
        kStandby,//待機
        kRun,//移動
    };
    float flameTime_;//フレームタイム
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
    //キャラごとの敵との距離感覚
    int distance_ = 1;

   

   
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

    float hp_ = AllyAIConstants::kMaxHPMPDisplay;
    float mp_ = AllyAIConstants::kMaxHPMPDisplay;
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
    bool henchmanSearch_ = false;
  
};

