#pragma once
#include "Model.h"
#include "Healer.h"
#include <ParticleSystem.h>

class HealerManager {
public:
	void Initialize();
	void Update();
	void Draw(const ViewProjection& camera);
	void RenderDirect(const ViewProjection& camera);
	Healer* GetHealer() { return healer_.get(); };
	const WorldTransform& GetWorldTransform();
	void SetViewProjection(const ViewProjection& viewProjection);


	Vector3 GetCanePos() { return healer_->GetWorldTransformCane().GetWorldPos(); }

	bool IsAttack() { return healer_->IsAttack(); }

	//プレイヤーに追従
	//  プレイヤーに追従
	void followPlayer(Vector3 playerPos);
	

	// 衝突を検出したら呼び出されるコールバック関数
	void OnAllyCollision(const WorldTransform& worldTransform);

	void SetParticlePos(Vector3 pos);

	void Dissolve() {
		threshold_ += 0.004f;
		for (int i = 0; i < 3; ++i) {
			HpModel_[i]->SetEdgeColor(Vector3{0.0f,-1.0f,-1.0f});
			nHpModel_[i]->SetEdgeColor(Vector3{ 0.0f,-1.0f,-1.0f });
			HpModel_[i]->SetThreshold(threshold_);
			nHpModel_[i]->SetThreshold(threshold_);
		}
		
	}

private:

	std::unique_ptr<Model> Model_;
	std::unique_ptr<Model> HpModel_[3];
	std::unique_ptr<Model> nHpModel_[3];
	std::unique_ptr<Model> CaneModel_;
	std::unique_ptr<Healer> healer_;

	//パーティクル
	std::unique_ptr<ParticleSystem> particle_;
	EmitterSphere emitter_;
	bool isParticle_ = false;

	bool isDead_ = false;
	float threshold_ = 0.0f;

	Vector3 playerPos_;
	
	// 味方の判定
	bool preHit_;
	bool isHit_;

	// 敵の判定
	bool preHitE_;
	bool isHitE_;
};