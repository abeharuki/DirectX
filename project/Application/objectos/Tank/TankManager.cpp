#include "TankManager.h"

void TankManager::Initialize() {
	
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/Tank", "Atlas.png", "tank.gltf"));

	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/character/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/character/HPG.png"));
	sprite2P_.reset(Sprite::Create("resources/character/2P.png"));
	spriteH_.reset(Sprite::Create("resources/character/H.png"));
	spriteM_.reset(Sprite::Create("resources/character/M.png"));
	spriteName_.reset(Sprite::Create("resources/Tank/tank.png"));

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
	worldTransformShadow_.scale = { 2.2f,2.2f,1.0f };

	tank_ = std::make_unique<Tank>();
	tank_->Initialize(animation_.get(),"Tank");

	worldTransformShadow_.translate = { tank_->GetWorldPosition().x,0.1f,tank_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();
}

void TankManager::Update() {
	particle_->SetEmitter(emitter_);
	if (isParticle_) {
		particle_->Update();
		if (!particle_->GetEmit()) {
			isParticle_ = false;
		}
	}
	
	//影の計算
	shadowColor_.w = 1 - (tank_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { tank_->GetWorldPosition().x,0.1f,tank_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	tank_->Update();
	tank_->SetPlayerPos(playerPos_);

	spriteHpSize_.x = tank_->GetHp();
	spriteMpSize_.x = tank_->GetMp();
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

void TankManager::Draw(const ViewProjection& camera) {
	tank_->Draw(camera);
	particle_->Draw(camera);

	shadowModel_->Draw(worldTransformShadow_, camera, false);
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

void TankManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}

