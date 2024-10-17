#include "PlayerManager.h"


void PlayerManager::Initialize() {

	
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));
	HammerModel_.reset(Model::CreateModelFromObj("resources/player/sword.obj", "resources/player/sword.png"));

	spriteRevival_.reset(Sprite::Create("resources/enemy/HP.png"));
	spriteRevivalG_.reset(Sprite::Create("resources/HPG.png"));

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/HPG.png"));
	sprite1P_.reset(Sprite::Create("resources/1P.png"));
	spriteH_.reset(Sprite::Create("resources/H.png"));
	spriteM_.reset(Sprite::Create("resources/M.png"));
	spriteName_.reset (Sprite::Create("resources/player.png"));
	for (int i = 0; i < 3; ++i) {
		HPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		MPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		HPnum_[i]->SetSize(Vector2{ 36.0f,36.0f });
		MPnum_[i]->SetSize(Vector2{ 36.0f,36.0f });
		HPnum_[i]->SetPosition(Vector2{ 1172.0f - (16.0f * i) ,385.0f });
		MPnum_[i]->SetPosition(Vector2{ 1172.0f - (16.0f * i) ,410.0f });
	}

	HPnum_[2]->SetTexture("resources/character/1.png");
	MPnum_[2]->SetTexture("resources/character/1.png");

	spriteHP_->SetPosition(Vector2{1106.0f,405.0f});
	spriteHPG_->SetPosition(Vector2{ 1106.0f,405.0f });
	spriteMP_->SetPosition(Vector2{ 1106.0f,430.0f });
	spriteMPG_->SetPosition(Vector2{ 1106.0f,430.0f });
	sprite1P_->SetPosition(Vector2{ 995.0f,373.0f });
	spriteH_->SetPosition(Vector2{ 1097.0f,383.0f });
	spriteM_->SetPosition(Vector2{ 1097.0f,408.0f });
	spriteName_->SetPosition(Vector2{ 995.0f,363.0f});

	spriteHPG_->SetSize(Vector2{ 100.0f,10.0f });
	spriteMPG_->SetSize(Vector2{ 100.0f,10.0f });
	sprite1P_->SetSize(Vector2{ 93.0f,85.0f });
	spriteH_->SetSize(Vector2{ 35.0f,35.0f });
	spriteM_->SetSize(Vector2{ 35.0f,35.0f });
	spriteName_->SetSize(Vector2{ 106.0f,50.0f });

	spriteHpSize_ = { 100.0f,10.0f };
	spriteMpSize_ = { 100.0f,10.0f };

	player_ = std::make_unique<Player>();
	player_->Initialize();


	worldTransformHead_ = player_->GetWorldTransform();

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
	

	revivalTransform_.scale = { 137.0f,22.0f,70.0f };
	revivalTransform_.translate = { 571.0f,650.0f,1.0f };
	spriteRevivalG_->SetSize(Vector2(120.0f, revivalTransform_.scale.y));
	spriteRevivalG_->SetPosition(Vector2(revivalTransform_.translate.x, revivalTransform_.translate.y));
	spriteRevival_->SetPosition(Vector2(revivalTransform_.translate.x, revivalTransform_.translate.y));
	
}

void PlayerManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHitH_ = isHitH_;
	isHitH_ = false;
	preHitR_ = isHitR_;
	isHitR_ = false;
	preHitT_ = isHitT_;
	isHitT_ = false;

	OnCollision();


	
	if (Input::PushKey(DIK_P)) {
		isDead_ = true;
	}

	particle_->SetEmitter(emitter_);
	if (isParticle_) {
		particle_->Update();
		if (!particle_->GetEmit()) {
			isParticle_ = false;
		}
	}

	//キャラクターの更新
	player_->Update();

	spriteHpSize_.x = player_->GetHp();
	spriteMpSize_.x = player_->GetMp();
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
	//体力低下で色の変化
	if (spriteHpSize_.x < 20) {
		hpColor_ = { 5.0f,0.0f,0.0f,1.0f };
	}
	else {
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

	//復活させる時の(今は使ってない)
	Revival();
	if (revivalTransform_.scale.x >= 120.0f) {
		revivalTransform_.scale.x = 120.0f;
	}
	spriteRevival_->SetSize(Vector2(revivalTransform_.scale.x, revivalTransform_.scale.y));

	ImGui::Begin("Sprite");
	ImGui::DragFloat2("Hpsize", &spriteHpSize_.x, 1.0f);
	ImGui::DragFloat2("Mpsize", &spriteMpSize_.x, 1.0f);
	ImGui::DragFloat4("HpColor", &hpColor_.x, 0.1f);
	ImGui::End();

};

void PlayerManager::Draw(const ViewProjection& camera) {
	
	player_->Draw(camera);
	particle_->Draw(camera);

	if (player_->GetBehavior() == Player::Behavior::kAttack) {
		HammerModel_->Draw(player_->GetWorldTransformHammer(), camera, false);
	}
};

void PlayerManager::DrawUI() {
	
	
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteMPG_->Draw();
	spriteMP_->Draw();
	sprite1P_->Draw();
	spriteH_->Draw();
	spriteM_->Draw();
	spriteName_->Draw();

	if (player_->GetIsDead()) {
		spriteRevivalG_->Draw();
		spriteRevival_->Draw();

	}

	
	for (int i = 0; i < 2; ++i) {
		HPnum_[i]->Draw();
		MPnum_[i]->Draw();
	}

	if (player_->GetHp() >= 100) {
		HPnum_[2]->Draw();
	}
	if (player_->GetMp() >= 100) {
		MPnum_[2]->Draw();
	}

}

// 衝突を検出したら呼び出されるコールバック関数
void PlayerManager::OnHCollision() {
	isHitH_ = true;
	if (isHitH_ != preHitH_) {
		//--hitCount_;
	}
}
void PlayerManager::OnRCollision() {
	isHitR_ = true;
	if (isHitR_ != preHitR_) {
		//--hitCount_;
	}
}
void PlayerManager::OnTCollision() {
	isHitT_ = true;
	if (isHitT_ != preHitT_) {
		//--hitCount_;

	}

}
void PlayerManager::OnCollision() {
	
};

void PlayerManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}


const WorldTransform& PlayerManager::GetWorldTransform() { return player_->GetWorldTransform(); }
Vector3 PlayerManager::katanaPos() { return player_->GetWorldTransformCollision().GetWorldPos(); }
void PlayerManager::SetViewProjection(const ViewProjection* viewProjection) {
	player_->SetViewProjection(viewProjection);
}