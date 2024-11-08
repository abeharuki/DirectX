#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "../../BehaviorTree/BehaviorTree.h"
#include "Enemy/Enemy.h"
#include <ParticleSystem.h>
#include "base/BaseCharacter.h"


/// <summary>
/// ゲームシーン
/// </summary>
class Healer : public Collider {

public: // メンバ関数
	~Healer();	
	void SetState(CharacterState newState) {
		state_ = newState;
	}//同じ

	CharacterState GetState() const {
		return state_;
	}//同じ
	void Initialize();//違う

	void Update();//違う

	void Draw(const ViewProjection& camera);//違う
	void NoDepthDraw(const ViewProjection& camera);//違う

	// 移動
	void MoveInitialize();//違う
	void MoveUpdate();//違う

	// ジャンプ
	void JumpInitialize();//違う
	void JumpUpdate();//違う

	// ノックバック
	void knockInitialize();//いらない
	void knockUpdate();//いらない

	// 攻撃
	void AttackInitialize();//違う
	void AttackUpdate();//違う

	//味方の回復
	void UniqueInitialize();//違う
	void UniqueUpdate();//違う

	//死亡
	void DeadInitialize();//違う
	void DeadUpdate();//違う


	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);//同じ
	
	//位置の初期化
	void InitPos();//違う

	void Dissolve() {
		animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
		threshold_ += 0.004f;
		animation_->SetThreshold(threshold_);
	}//同じ

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;//違う


	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;//同じ
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }//おなじ
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }//同じ
	WorldTransform& GetWorldTransformCane() { return worldTransformCane_; }//このクラスだけ


	bool IsAttack() { return workAttack_.isAttack; }//違う
	float GetHp() { return hp_; }//同じ
	float GetMp() { return mp_; }//同じ
	bool GetHeal() { return heal_; }//このクラスだけ
	bool GetAllHeal() { return allHeal_; }//このクラスだけ
	bool GetOneHeal() { return oneHeal_; }//このクラスだけ
	float GetHealAmount() { return healAmount_; }//このクラスだけ
	bool IsDead() { return isDead_; }//同じ


	/*-----セッター-----*/
	//敵の情報の受け取り
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }

	void SetLight(DirectionLight directionLight) {
		animation_->DirectionalLightDraw(directionLight);
		DirectionLight light;
		light = directionLight;
		light.direction *= -1.0f;
		for (int i = 0; i < 4; ++i) {
			magicCircle_[i]->DirectionalLightDraw(light);
		}

	}//違う

	void SetOperation(bool flag) { operation_ = flag; }//同じ
	void SetViewProjection(const ViewProjection& viewProjection) {viewProjection_ = viewProjection;}//同じ

	/*--------このクラス特有の関数-------*/
	//座標の受け取り
	void SetPos(Vector3 playerPos, Vector3 renjuPos, Vector3 tankPos) {
		pos[0] = playerPos;
		pos[1] = renjuPos;
		pos[2] = tankPos;
	}
	//味方キャラのHpの受け取り
	void SetHp(Vector3 hp) { 
		playerHp_ = hp.x;
		renjuHp_ = hp.y;
		tankHp_ = hp.z;
	}
	void SetHeal(float heal) {
		if (!isDead_) {
			hp_ += heal;
		}
		
	}
	
	

private:
	//敵を探す
	void searchTarget();//同じ
	//敵の視野内にいるかどうか
	void IsVisibleToEnemy();//同じ
	//逃げる方向
	void RunAway();//同じ

	// パーツ親子関係
	void Relationship();//違う

	//次の状態遷移をノードから検索
	CharacterState NextState(std::string name,int outputNum);//同じ(関数の引数にユニークスキル名を追加)


private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	WorldTransform worldTransformCane_;
	WorldTransform worldTransformMagicCircle_[4];
	WorldTransform worldTransformHeal_[4];
	ViewProjection viewProjection_;

	//ダメージ表示
	WorldTransform worldTransformNum_;
	std::unique_ptr<Model> damageModel_;
	float alpha_;
	Vector3 numMove_;

	//アニメーション
	std::unique_ptr<Animations>animation_;
	int animationNumber_;
	enum AnimationNumber {
		animeAttack,//攻撃
		death,//死亡
		jump,//ジャンプ
		standby,//待機
		run,//移動
	};

	float flameTime_;

	// 目標の角度
	float destinationAngleY_ = 0.0f;

	enum Character {
		healer,
		player,
		renju,
		tank,
	};
	//魔法陣
	std::unique_ptr<Model> magicCircle_[4];
	std::unique_ptr<Model> healModel_[4];
	float t_[4];//ディゾルブ
	Vector3 pos[3];
	float healAlph_[4];
	Vector3 healNumMove_[4];

	//パーティクル
	std::vector<std::unique_ptr<ParticleSystem>> particle_;
	std::vector<EmitterSphere> emitter_{};

	//ビヘイビアツリー
	BehaviorTree<Healer>* behaviorTree_;
	CharacterState state_;
	CharacterState previousState_;

	// 攻撃用ワーク
	struct WorkAttack {
		uint32_t attackParameter_ = 0;
		int32_t comboIndex = 0;
		int32_t inComboPhase = 0;
		bool comboNext = false;
		bool isAttack = false;
		bool isFinalAttack = false;
	};

	WorkAttack workAttack_;

	// 攻撃の時間
	const uint32_t behaviorAttackTime = 15;
	//攻撃ができるようになるまでの
	int coolTime = 60;

	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	//プレイヤー座標
	float minDistance_ = 8.0f;
	bool followPlayer_;
	

	// 敵を探すフラグ
	bool searchTarget_ = false;

	uint32_t nockTime_;
	bool nockBack_;

	//体力
	float hp_ = 100.0f;
	//マジックポイント
	float mp_ = 100.0f;
	//回復フラグ
	bool allHeal_;//全体回復
	bool oneHeal_;//個人回復
	bool heal_;//ヒールする瞬間
	bool healAnimation_;//ヒールの演出

	//回復量
	float healAmount_;
	//復活時間
	int revivalCount_ = 0;

	bool preHit_;
	bool isHit_;
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

	//死亡フラグ
	bool isDead_ = false;
	float threshold_;

	//敵情報
	Enemy* enemy_;
	
	//作戦
	bool operation_;

	int jumpCount_;
	float enemylength_;

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

	//味方のHp
	float playerHp_;
	float renjuHp_;
	float tankHp_;

	//ノードエディター
	Editor::NodeEditor editor_;
};