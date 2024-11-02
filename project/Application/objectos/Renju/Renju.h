#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include "RenjuBullet.h"
#include <CollisionManager/Collider.h>
#include <Animation/Animation.h>
#include "../../BehaviorTree/BehaviorTree.h"
#include "Enemy/Enemy.h"

/// <summary>
/// ゲームシーン
/// </summary>
class Renju : public Collider {

public: // メンバ関数
	~Renju();
	
	
	void Initialize();
	
	void Update();


	void Draw(const ViewProjection& camera);
	void NoDepthDraw(const ViewProjection& camera);

	// 移動
	void MoveInitialize();
	void MoveUpdate();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdate();

	// ノックバック
	void knockInitialize();
	void knockUpdate();

	// 攻撃
	void AttackInitialize();
	void AttackUpdate();

	//味方にバフをかけるか技を打つか
	void UniqueInitialize();
	void UniqueUpdate();

	//死亡
	void DeadInitialize();
	void DeadUpdate();

	// プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	

	//位置の初期化
	void InitPos();

	void Dissolve() {
		animation_->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
		threshold_ += 0.004f;
		animation_->SetThreshold(threshold_);
	}

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision(Collider* collider) override;

	/*-----ゲッター-----*/
	const Vector3 GetWorldPosition() const override;
	Vector3 GetLocalPosition();
	const WorldTransform& GetWorldTransform() const override { return worldTransformBase_; }
	WorldTransform& GetWorldTransformHead() { return worldTransformHead_; }
	// 弾リストの取得
	const std::list<RenjuBullet*>& GetBullets() const { return bullets_; }
	bool GetHitBullet() { return hitBullet_; }
	float GetHp() { return hp_; }
	float GetMp() { return mp_; }
	bool IsDead() { return isDead_; }

	CharacterState GetState() const {
		return state_;
	}

	/*-----セッター-----*/
	void SetViewProjection(const ViewProjection& viewProjection) {
		viewProjection_ = viewProjection;
	}

	//敵の情報の受け取り
	void SetEnemy(Enemy* enemy) { enemy_ = enemy; }
	void SetHeal(float heal) {
		if (!isDead_) {
			hp_ += heal;
		}
	}

	void SetLight(DirectionLight directionLight) { animation_->DirectionalLightDraw(directionLight); }
	void SetOperation(bool flag) { operation_ = flag; }

	void SetState(CharacterState newState) {
		state_ = newState;
	}


private:
	//敵を探す
	void searchTarget(Vector3 enemyPos);
	//敵の視野内にいるかどうか
	void IsVisibleToEnemy();
	//逃げる方向
	void RunAway();

	// パーツ親子関係
	void Relationship();

private: // メンバ変数
	WorldTransform worldTransformBase_;
	WorldTransform worldTransformHead_;
	ViewProjection viewProjection_;

	//ダメージ表示
	WorldTransform worldTransformNum_;
	std::unique_ptr<Model> damageModel_;
	float alpha_;
	Vector3 numMove_;
	
	std::unique_ptr<Model> bulletModel_;
	std::unique_ptr<Animations>animation_;
	int animationNumber_;
	enum AnimationNumber {
		//animeAttack,//攻撃
		death,
		jump,//ジャンプ
		run,//移動
		standby,//待機
	};

	float flameTime_;
	// 目標の角度
	float destinationAngleY_ = 0.0f;

	//ビヘイビアツリー
	BehaviorTree<Renju>* behaviorTree_;
	CharacterState state_;
	CharacterState previousState_;

	// 速度
	Vector3 velocity_ = {};
	// 味方の押し出し処理
	Vector3 allyVelocity;

	//攻撃フラグ
	bool attack_;
	bool hitBullet_ = false;

	// プレイヤー座標
	float minDistance_ = 10.0f;
	bool followPlayer_ = false;
	
	//敵を探すフラグ
	bool searchTarget_ = false;

	//弾
	std::list<RenjuBullet*> bullets_;
	// 攻撃時間
	int fireTimer_ = 20;
	//攻撃ができるようになるまでの
	int coolTime = 60;

	Vector3 enemyPos_;
	uint32_t nockTime_;
	bool nockBack_;
	//体力
	float hp_ = 100.0f;
	//マジックポイント
	float mp_ = 100.0f;

	//復活時間
	int revivalCount_ = 0;

	//ヒット確認
	bool preHit_;
	bool isHit_;
	bool isHitPlayer_ = false;
	bool preHitPlayer_ = false;

	//死亡フラグ
	bool isDead_ = false;
	float threshold_ = 0;

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
	float angleRange_ = 30.0f * kDegreeToRandian;
	//敵の攻撃範囲ないかどうか
	bool isArea_ = false;
};