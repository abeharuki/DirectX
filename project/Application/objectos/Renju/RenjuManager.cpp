#include "RenjuManager.h"

void RenjuManager::Initialize() {

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/HPG.png"));
	sprite3P_.reset(Sprite::Create("resources/3P.png"));
	spriteH_.reset(Sprite::Create("resources/H.png"));
	spriteM_.reset(Sprite::Create("resources/M.png"));
	spriteName_.reset(Sprite::Create("resources/renju.png"));

	renju_ = std::make_unique<Renju>();
	renju_->Initialize();

	spriteHP_->SetPosition(Vector2{ 1106.0f,545.0f });
	spriteHPG_->SetPosition(Vector2{ 1106.0f,545.0f });
	spriteMP_->SetPosition(Vector2{ 1106.0f,570.0f });
	spriteMPG_->SetPosition(Vector2{ 1106.0f,570.0f });
	sprite3P_->SetPosition(Vector2{ 995.0f,513.0f });
	spriteH_->SetPosition(Vector2{ 1097.0f,523.0f });
	spriteM_->SetPosition(Vector2{ 1097.0f,548.0f });
	spriteName_->SetPosition(Vector2{ 995.0f,503.0f });

	spriteHPG_->SetSize(Vector2{ 100.0f,10.0f });
	spriteMPG_->SetSize(Vector2{ 100.0f,10.0f });
	sprite3P_->SetSize(Vector2{ 93.0f,85.0f });
	spriteH_->SetSize(Vector2{ 35.0f,35.0f });
	spriteM_->SetSize(Vector2{ 35.0f,35.0f });
	spriteName_->SetSize(Vector2{ 106.0f,50.0f });

	spriteHpSize_ = { 100.0f,10.0f };
	spriteMpSize_ = { 100.0f,10.0f };

	emitter_ = {
		.translate{0,0,0},
		.count{10},
		.frequency{0.02f},
		.frequencyTime{0.0f},
		.scaleRange{.min{1,1,1},.max{1,1,1}},
		.translateRange{.min{0,0,0},.max{0,0,0}},
		.colorRange{.min{1,1,1},.max{1,1,1}},
		.lifeTimeRange{.min{0.5f},.max{0.5f}},
		.velocityRange{.min{-0.2f,-0.2f,-0.2f},.max{0.2f,0.2f,0.2f}},
	};
	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	
}

void RenjuManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	// 敵の判定
	preHitE_ = isHitE_;
	isHitE_ = false;

	
	particle_->SetEmitter(emitter_);
	if (isParticle_) {
		particle_->Update();
		if (!particle_->GetEmit()) {
			isParticle_ = false;
		}
	
	}

	renju_->Update();
	renju_->followPlayer(playerPos_);

	spriteHpSize_.x = renju_->GetHp();
	spriteHP_->SetSize(spriteHpSize_);
	spriteMP_->SetSize(spriteMpSize_);

	spriteHP_->SetColor(hpColor_);
	if (spriteHpSize_.x < 20) {
		hpColor_ = { 5.0f,0.0f,0.0f,1.0f };
	}
	else {
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}
	particle_->DebugParameter();


};

void RenjuManager::Draw(const ViewProjection& camera) {
	renju_->Draw(camera);
	particle_->Draw(camera);
}
void RenjuManager::DrawUI(){
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteMPG_->Draw();
	spriteMP_->Draw();
	sprite3P_->Draw();
	spriteH_->Draw();
	spriteM_->Draw();
	spriteName_->Draw();
};

void RenjuManager::RenderDirect(const ViewProjection& camera) {
	
}

void RenjuManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
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