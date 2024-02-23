#include "RenjuManager.h"

void RenjuManager::Initialize() {

	Model_.reset(
		Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));

	renju_ = std::make_unique<Renju>();
	renju_->Initialize();

	worldTransformBase_.Initialize();
	worldTransformBase_ = renju_->GetWorldTransform();

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
	worldTransformBase_ = renju_->GetWorldTransform();
	renju_->followPlayer(playerPos_);
	renju_->searchTarget(enemyPos_);
};

void RenjuManager::Draw(const ViewProjection& camera) {
	Model_->Draw(renju_->GetWorldTransformHead(), camera, false);
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