#pragma once
#include "Enemy.h"
#include <Sphere.h>

/**
 * @file EnemyManager
 * @brief 敵キャラクターを管理するクラス
 */
class EnemyManager {
public:
	//キャラクターの生成
	void Initialize();
	//キャラクターの更新
	void Update();
	//キャラクターの描画
	void Draw(const ViewProjection& camera);
	//深度値のないものの描画
	void NoDepthDraw(const ViewProjection& camera);
	//キャラクターUIの描画
	void DrawUI();

	//ダメージ数値のアニメーション
	void DamageNumMath();

	// 衝突を検出したら呼び出されるコールバック関数
	void OnCollision();
	void OnHealerCollision();
	void OnTankCollision();
	void OnRenjuCollision(bool skill);

	//ビルボードの計算
	void Billboard();

	//パーティクルを強制的に止める
	void Stop() { enemy_->StopParticle(); }

	/*-----ゲッター-----*/
	Enemy* GetEnemy() { return enemy_.get(); };
	bool IsAttack() { return enemy_->IsAttack(); }
	bool IsClear() { return enemy_->IsClear(); }

	const WorldTransform& GetWorldTransform();
	Vector3 GetRockWorldPos();

	/*-----セッター-----*/
	void SetPlayerPos(Vector3 pos) { 
		if (!enemy_->IsBehaberAttack()){
			enemy_->SetPlayerPos(pos);
		}
		playerPos_ = pos;
	};
	void SetHealerPos(Vector3 pos) { 
		if (!enemy_->IsBehaberAttack()) {
			enemy_->SetHealerPos(pos);
		}
	};
	void SetRenjuPos(Vector3 pos) { 
		if (!enemy_->IsBehaberAttack()) {
			enemy_->SetRenjuPos(pos);
		}
	};
	void SetTankPos(Vector3 pos) { 
		if (!enemy_->IsBehaberAttack() || enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kBreath) {
			enemy_->SetTankPos(pos);
		}
		
	};
	void SetCamera(const ViewProjection& camera) { 
		camera_ = camera; 
	
	}


private:
	WorldTransform worldTransformName_;
	WorldTransform worldTransformShadow_;
	std::vector<WorldTransform> worldTransformNum_;
	ViewProjection camera_;

	std::unique_ptr<Model> shadowModel_;
	std::unique_ptr<Sphere> rockModel_;
	std::unique_ptr<Model> bulletModel_;
	//ダメージを食らったときのダメージ表示モデル
	std::unique_ptr<Model> damageNumModel_[4];
	enum damageNum_ {
		playerNum,//プレイヤーのダメージ
		healerNum,//ヒーラーのダメージ
		renjuNum,//レンジャーのダメージ
		tankNum,//タンクーののダメージ
		kDamageNumMax,
	};

	//ダメージのアルファ値
	float playerNumAlpha_;
	float healerNumAlpha_;
	float renjuNumAlpha_;
	float tankNumAlpha_;

	//ダメージの移動数値
	Vector3 playerNumMove_;
	Vector3 healerNumMove_;
	Vector3 renjuNumMove_;
	Vector3 tankNumMove_;

	std::unique_ptr<Enemy> enemy_;

	std::unique_ptr<Model> nameModel_;
	float hp_;

	Vector4 color_;//名前の色
	Vector4 shadowColor_ = { 0.2f,0.2f,0.2f,1.f };//影の色

	Vector3 playerPos_;
	float playerLength_;//プレイヤーとの距離
	float nameScale_;

	bool isDead_ = false;

	//各キャラのヒット確認
	bool preHit_;
	bool isHit_;

	bool preHitH_;
	bool isHitH_;

	bool preHitR_;
	bool isHitR_;

	bool preHitT_;
	bool isHitT_;

};
