#include "TankManager.h"

void TankManager::Initialize() {
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
	isParticle_ = false;


	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/HPG.png"));
	sprite2P_.reset(Sprite::Create("resources/2P.png"));
	spriteH_.reset(Sprite::Create("resources/H.png"));
	spriteM_.reset(Sprite::Create("resources/M.png"));
	spriteName_.reset(Sprite::Create("resources/tank.png"));

	for (int i = 0; i < 3; ++i) {
		HPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		MPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		HPnum_[i]->SetSize(Vector2{ 36.0f,36.0f });
		MPnum_[i]->SetSize(Vector2{ 36.0f,36.0f });
		HPnum_[i]->SetPosition(Vector2{ 1172.0f - (16.0f * i) ,455.0f });
		MPnum_[i]->SetPosition(Vector2{ 1172.0f - (16.0f * i) ,480.0f });
	}

	HPnum_[2]->SetTexture("resources/character/1.png");
	MPnum_[2]->SetTexture("resources/character/1.png");

	
	spriteHP_->SetPosition(Vector2{ 1106.0f,475.0f });
	spriteHPG_->SetPosition(Vector2{ 1106.0f,475.0f });
	spriteMP_->SetPosition(Vector2{ 1106.0f,500.0f });
	spriteMPG_->SetPosition(Vector2{ 1106.0f,500.0f });
	sprite2P_->SetPosition(Vector2{ 995.0f,443.0f });
	spriteH_->SetPosition(Vector2{ 1097.0f,453.0f });
	spriteM_->SetPosition(Vector2{ 1097.0f,478.0f });
	spriteName_->SetPosition(Vector2{ 995.0f,433.0f });

	spriteHPG_->SetSize(Vector2{ 100.0f,10.0f });
	spriteMPG_->SetSize(Vector2{ 100.0f,10.0f });
	sprite2P_->SetSize(Vector2{ 93.0f,85.0f });
	spriteH_->SetSize(Vector2{ 35.0f,35.0f });
	spriteM_->SetSize(Vector2{ 35.0f,35.0f });
	spriteName_->SetSize(Vector2{ 106.0f,50.0f });
	
	spriteHpSize_ = { 100.0f,10.0f };
	spriteMpSize_ = { 100.0f,10.0f };


	tank_ = std::make_unique<Tank>();
	tank_->Initialize();

}

void TankManager::Update() {
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
	
	//キャラクターの更新
	tank_->Update();
	tank_->followPlayer(playerPos_);
	

	spriteHpSize_.x = tank_->GetHp();
	spriteMpSize_.x = tank_->GetMp();
	spriteHP_->SetSize(spriteHpSize_);
	spriteMP_->SetSize(spriteMpSize_);

	//HP,MP表示の計算
	if (spriteHpSize_.x < 100.0f) {
		int HPnum = int(spriteHpSize_.x);
		HPnum %= 10;
		HPnum_[0]->SetTexture("resources/character/" + std::to_string(HPnum) + ".png");
		HPnum_[1]->SetTexture("resources/character/" + std::to_string(int(spriteHpSize_.x / 10)) + ".png");
	}
	else {
		HPnum_[0]->SetTexture("resources/character/0.png");
		HPnum_[1]->SetTexture("resources/character/0.png");
	}

	if (spriteMpSize_.x < 100.0f) {
		int MPnum = int(spriteMpSize_.x);
		MPnum %= 10;
		MPnum_[0]->SetTexture("resources/character/" + std::to_string(MPnum) + ".png");
		MPnum_[1]->SetTexture("resources/character/" + std::to_string(int(spriteMpSize_.x / 10)) + ".png");
	}
	else {
		MPnum_[0]->SetTexture("resources/character/0.png");
		MPnum_[1]->SetTexture("resources/character/0.png");
	}


	spriteHP_->SetColor(hpColor_);
	if (spriteHpSize_.x < 20) {
		hpColor_ = { 5.0f,0.0f,0.0f,1.0f };
	}
	else {
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

	
};

void TankManager::Draw(const ViewProjection& camera) {
	tank_->Draw(camera);
	particle_->Draw(camera);
}
void TankManager::DrawUI(){
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteMPG_->Draw();
	spriteMP_->Draw();
	sprite2P_->Draw();
	spriteH_->Draw();
	spriteM_->Draw();
	spriteName_->Draw();

	for (int i = 0; i < 2; ++i) {
		HPnum_[i]->Draw();
		MPnum_[i]->Draw();
	}

	if (tank_->GetHp() >= 100) {
		HPnum_[2]->Draw();
	}
	if (tank_->GetMp() >= 100) {
		MPnum_[2]->Draw();
	}
};

void TankManager::RenderDirect(const ViewProjection& camera) {
	camera;
}

// 衝突を検出したら呼び出されるコールバック関数
void TankManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		tank_->OnAllyCollision(worldTransform);
	}

}
// 衝突を検出したら呼び出されるコールバック関数
void TankManager::OnCollision(const WorldTransform& worldTransform) {
	isHitE_ = true;
	if (isHitE_ != preHitE_) {
		tank_->OnCollision(worldTransform);
	}
};


void TankManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}


void TankManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

const WorldTransform& TankManager::GetWorldTransform() { return tank_->GetWorldTransform(); }
void TankManager::SetViewProjection(const ViewProjection& viewProjection) {
	tank_->SetViewProjection(viewProjection);
}