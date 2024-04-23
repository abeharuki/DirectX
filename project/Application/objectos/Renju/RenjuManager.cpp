#include "RenjuManager.h"

void RenjuManager::Initialize() {

	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	for (int i = 0; i < 3; i++) {
		nHpModel_[i].reset(Model::CreateModelFromObj("resources/plane.obj", "resources/Player/life0.png"));
		HpModel_[i].reset(Model::CreateModelFromObj("resources/plane.obj", "resources/Player/life1.png"));
	}

	renju_ = std::make_unique<Renju>();
	renju_->Initialize();


	emitter_.transform = {
		{0.8f, 0.8f, 0.8f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};
	emitter_.count = 10;
	emitter_.frequencyTime = 0;

	particle_.reset(Particle::Create("resources/particle/circle.png", emitter_));

}

void RenjuManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	// 敵の判定
	preHitE_ = isHitE_;
	isHitE_ = false;

	for (RenjuParticle* particle : particles_) {

		particle->Update();
	}
	renju_->Update();
	renju_->followPlayer(playerPos_);
	renju_->searchTarget(enemyPos_);
};

void RenjuManager::Draw(const ViewProjection& camera) {
	//Model_->Draw(renju_->GetWorldTransformHead(), camera, false);
	renju_->Draw(camera);
	if (renju_->GetHitCount() >= 3) {
		HpModel_[0]->Draw(renju_->GetWorldTransfromHp(0), camera, false);

	}
	else {
		nHpModel_[0]->Draw(renju_->GetWorldTransfromHp(0), camera, false);
	}
	if (renju_->GetHitCount() >= 2) {
		HpModel_[1]->Draw(renju_->GetWorldTransfromHp(1), camera, false);

	}
	else {
		nHpModel_[1]->Draw(renju_->GetWorldTransfromHp(1), camera, false);
	}
	if (renju_->GetHitCount() >= 1) {
		HpModel_[2]->Draw(renju_->GetWorldTransfromHp(2), camera, false);

	}
	else {
		nHpModel_[2]->Draw(renju_->GetWorldTransfromHp(2), camera, false);
	}
	renju_->Draw(camera);

	for (RenjuParticle* particle : particles_) {
		particle->Draw(camera);
	}
};


void RenjuManager::SetParticlePos(Vector3 pos) {
	// デスフラグが立った弾を削除
	particles_.remove_if([](RenjuParticle* particle) {

		delete particle;
		return true;

		});
	RenjuParticle* newParticle = new RenjuParticle();
	newParticle->Initialize(particle_.get(), pos);

	particles_.push_back(newParticle);
}

// 衝突を検出したら呼び出されるコールバック関数
void RenjuManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		renju_->OnAllyCollision(worldTransform);
	}

};

// 衝突を検出したら呼び出されるコールバック関数
void RenjuManager::OnCollision(const WorldTransform& worldTransform) {
	isHitE_ = true;
	if (isHitE_ != preHitE_) {
		renju_->OnCollision(worldTransform);
	}
};
void RenjuManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& RenjuManager::GetWorldTransform() { return renju_->GetWorldTransform(); }
void RenjuManager::SetViewProjection(const ViewProjection& viewProjection) {
	renju_->SetViewProjection(viewProjection);
}