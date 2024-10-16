#pragma once
#include "Enemy.h"
#include "Model.h"
#include <Sphere.h>


class EnemyManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void NoDepthDraw(const ViewProjection& camera);
	
	void DrawUI();

	void DamageNumMath();

	Enemy* GetEnemy() { return enemy_.get(); };
	bool IsAttack() { return enemy_->isAttack(); }
	bool IsClear() { return enemy_->isClear(); }

	void SetPlayerPos(Vector3 pos) { enemy_->SetPlayerPos(pos); };
	void SetHealerPos(Vector3 pos) { enemy_->SetHealerPos(pos); };
	void SetRenjuPos(Vector3 pos) { enemy_->SetRenjuPos(pos); };
	void SetTankPos(Vector3 pos) { enemy_->SetTankPos(pos); };
	void SetCamera(const ViewProjection& camera) { 
		camera_ = camera; 
	
	}

	const WorldTransform& GetWorldTransform();
	Vector3 GetRockWorldPos();
	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
	void OnHealerCollision();
	void OnTankCollision();
	void OnRenjuCollision();

	void Billboard();

private:
	WorldTransform worldTransformName_;
	std::vector<WorldTransform> worldTransformNum_;
	ViewProjection camera_;

	//std::unique_ptr<Model> rockModel_;
	std::unique_ptr<Sphere> rockModel_;
	std::unique_ptr<Model> bulletModel_;
	//ダメージを食らったときのダメージ表示モデル
	std::unique_ptr<Model> damageNumModel_[4];
	enum damageNum_ {
		playerNum,//プレイヤーのダメージ
		healerNum,//ヒーラーのダメージ
		renjuNum,//レンジャーのダメージ
		tankNum,//タンクーののダメージ
		kdamageNumMax,
	};

	float playerNumAlpha_;
	float healerNumAlpha_;
	float renjuNumAlpha_;
	float tankNumAlpha_;

	Vector3 playerNumMove_;
	Vector3 healerNumMove_;
	Vector3 renjuNumMove_;
	Vector3 tankNumMove_;

	std::unique_ptr<Enemy> enemy_;

	std::unique_ptr<Model> nameModel_;
	Transform HpTransform_;

	Vector4 color_;

	bool isDead_ = false;


	bool preHit_;
	bool isHit_;

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;

};
