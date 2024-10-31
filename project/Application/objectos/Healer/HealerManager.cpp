#include "HealerManager.h"

void HealerManager::Initialize() {
	
	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/character/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/character/HPG.png"));
	sprite4P_.reset(Sprite::Create("resources/character/4P.png"));
	spriteH_.reset(Sprite::Create("resources/character/H.png"));
	spriteM_.reset(Sprite::Create("resources/character/M.png"));
	spriteName_.reset(Sprite::Create("resources/Healer/healer.png"));
	StaffModel_.reset(Model::CreateModelFromObj("resources/Healer/weapon.obj", "resources/Healer/staff.png"));

	for (int i = 0; i < 3; ++i) {
		HPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		MPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		HPnum_[i]->SetSize(Vector2{ 36.0f,36.0f });
		MPnum_[i]->SetSize(Vector2{ 36.0f,36.0f });
		HPnum_[i]->SetPosition(Vector2{ 1172.0f - (16.0f * i) ,595.0f });
		MPnum_[i]->SetPosition(Vector2{ 1172.0f - (16.0f * i) ,620.0f });
	}

	HPnum_[2]->SetTexture("resources/character/1.png");
	MPnum_[2]->SetTexture("resources/character/1.png");

	spriteHP_->SetPosition(Vector2{ 1106.0f,615.0f });
	spriteHPG_->SetPosition(Vector2{ 1106.0f,615.0f });
	spriteMP_->SetPosition(Vector2{ 1106.0f,640.0f });
	spriteMPG_->SetPosition(Vector2{ 1106.0f,640.0f });
	sprite4P_->SetPosition(Vector2{ 995.0f,583.0f });
	spriteH_->SetPosition(Vector2{ 1097.0f,593.0f });
	spriteM_->SetPosition(Vector2{ 1097.0f,618.0f });
	spriteName_->SetPosition(Vector2{ 995.0f,573.0f });

	spriteHPG_->SetSize(Vector2{ 100.0f,10.0f });
	spriteMPG_->SetSize(Vector2{ 100.0f,10.0f });
	sprite4P_->SetSize(Vector2{ 93.0f,85.0f });
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
	
	isParticle_ = false;

	
	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = { -1.571f,0.0f,0.0f };
	worldTransformShadow_.scale = { 1.8f,1.8f,1.0f };

	healer_ = std::make_unique<Healer>();
	healer_->Initialize();

	worldTransformShadow_.translate = { healer_->GetWorldPosition().x,0.1f,healer_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void HealerManager::Update() {
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
	

	//影の計算
	shadowColor_.w = 1 - (healer_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { healer_->GetWorldPosition().x,0.1f,healer_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	healer_->Update();
	healer_->followPlayer(playerPos_);

	//ゲージの増減
	spriteHpSize_.x = healer_->GetHp();
	spriteMpSize_.x = healer_->GetMp();
	spriteHP_->SetSize(spriteHpSize_);
	spriteMP_->SetSize(spriteMpSize_);

	//HP,MP表示の計算
	if (spriteHpSize_.x < 100.0f) {
		int HPnum = int(spriteHpSize_.x);
		HPnum %= 10;
		if (spriteHpSize_.x >= 0) {
			HPnum_[0]->SetTexture("resources/character/" + std::to_string(HPnum) + ".png");
			HPnum_[1]->SetTexture("resources/character/" + std::to_string(int(spriteHpSize_.x / 10)) + ".png");
		}
		else {
			HPnum_[0]->SetTexture("resources/character/0.png");
			HPnum_[1]->SetTexture("resources/character/0.png");
		}
		
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
	for (int i = 0; i < 3; ++i) {
		HPnum_[i]->SetColor(hpNumColor_);
		MPnum_[i]->SetColor(hpNumColor_);
	}


	if (spriteHpSize_.x < 20) {
		hpColor_ = { 5.0f,0.0f,0.0f,1.0f };
		hpNumColor_ = { 5.0f,0.0f,0.0f,1.0f };

	}
	else if (spriteHpSize_.x <= 50) {
		hpNumColor_ = { 1.0f,0.2f,0.0f,1.0f };
	}
	else {
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
		hpNumColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

};

void HealerManager::Draw(const ViewProjection& camera) {
	healer_->Draw(camera);
	particle_->Draw(camera);
	
	shadowModel_->Draw(worldTransformShadow_, camera, false);

	if (healer_->IsAttack()) {
		StaffModel_->Draw(healer_->GetWorldTransformCane(), camera, false);
	}
	
}


void HealerManager::DrawUI(){
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteMPG_->Draw();
	spriteMP_->Draw();
	sprite4P_->Draw();
	spriteH_->Draw();
	spriteM_->Draw();
	spriteName_->Draw();

	for (int i = 0; i < 2; ++i) {
		HPnum_[i]->Draw();
		MPnum_[i]->Draw();
	}

	if (healer_->GetHp() == 100) {
		HPnum_[2]->Draw();
	}
	if (healer_->GetMp() == 100) {
		MPnum_[2]->Draw();
	}
};

void HealerManager::RenderDirect(const ViewProjection& camera) {
	camera;
};

void HealerManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

void HealerManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		healer_->OnAllyCollision(worldTransform);
	}


}



void HealerManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}



const WorldTransform& HealerManager::GetWorldTransform() { return healer_->GetWorldTransform(); }
void HealerManager::SetViewProjection(const ViewProjection& viewProjection) {
	healer_->SetViewProjection(viewProjection);
}