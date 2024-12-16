#include "PlayerManager.h"
#include <ParticleManager.h>


void PlayerManager::Initialize() {

	
	
	HammerModel_.reset(Model::CreateModelFromObj("resources/player/sword.obj", "resources/player/sword.png"));
	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));

	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	spriteRevival_.reset(Sprite::Create("resources/enemy/HP.png"));
	spriteRevivalG_.reset(Sprite::Create("resources/character/HPG.png"));

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/character/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/character/HPG.png"));
	sprite1P_.reset(Sprite::Create("resources/character/1P.png"));
	spriteH_.reset(Sprite::Create("resources/character/H.png"));
	spriteM_.reset(Sprite::Create("resources/character/M.png"));
	spriteName_.reset (Sprite::Create("resources/Player/player.png"));
	for (int i = 0; i < 3; ++i) {
		HPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		MPnum_[i].reset(Sprite::Create("resources/character/0.png"));
		HPnum_[i]->SetSize(PlayerConstants::kHpNumSize);
		MPnum_[i]->SetSize(PlayerConstants::kMpNumSize);
		HPnum_[i]->SetPosition(Vector2{ PlayerConstants::kHpNumInitPos.x - (PlayerConstants::kHpNumPosOffset * i) ,PlayerConstants::kHpNumInitPos.y});
		MPnum_[i]->SetPosition(Vector2{ PlayerConstants::kMpNumInitPos.x - (PlayerConstants::kMpNumPosOffset * i) ,PlayerConstants::kMpNumInitPos.y});
	}

	HPnum_[2]->SetTexture("resources/character/1.png");
	MPnum_[2]->SetTexture("resources/character/1.png");

	spriteHP_->SetPosition(PlayerConstants::kHpSpritePos);
	spriteHPG_->SetPosition(PlayerConstants::kHpSpritePos);
	spriteMP_->SetPosition(PlayerConstants::kMpSpritePos);
	spriteMPG_->SetPosition(PlayerConstants::kMpSpritePos);
	sprite1P_->SetPosition(PlayerConstants::k1PPos);
	spriteH_->SetPosition(PlayerConstants::kHPos);
	spriteM_->SetPosition(PlayerConstants::kMPos);
	spriteName_->SetPosition(PlayerConstants::kNamePos);

	spriteHPG_->SetSize(PlayerConstants::kHpSpriteSize);
	spriteMPG_->SetSize(PlayerConstants::kMpSpriteSize);
	sprite1P_->SetSize(PlayerConstants::k1PSize);
	spriteH_->SetSize(PlayerConstants::kHSize);
	spriteM_->SetSize(PlayerConstants::kMSize);
	spriteName_->SetSize(PlayerConstants::kNameSize);

	spriteHpSize_ = PlayerConstants::kHpSpriteSize;
	spriteMpSize_ = PlayerConstants::kMpSpriteSize;

	player_ = std::make_unique<Player>();
	player_->Initialize();


	worldTransformHead_ = player_->GetWorldTransform();
	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = PlayerConstants::kShadowRotate;
	worldTransformShadow_.scale = PlayerConstants::kShadowScale;
	worldTransformShadow_.translate = { player_->GetWorldPosition().x,PlayerConstants::kShadowHeight,player_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	emitter_ = {
		.translate{0,0,0},
		.count{10},
		.frequency{0.02f},
		.frequencyTime{0.0f},
		.scaleRange{.min{1,1,1},.max{1,1,1}},
		.translateRange{.min{0,0,0},.max{0,0,0}},
		.colorRange{.min{1,1,1},.max{1,1,1}},
		.alphaRange{.min{1.0f},.max{1.0f}},
		.lifeTimeRange{.min{0.5f},.max{0.5f}},
		.velocityRange{.min{-0.2f,-0.2f,-0.2f},.max{0.2f,0.2f,0.2f}},
	};
	particle_ = ParticleManager::Create("resources/particle/circle.png",8);

	isParticle_ = false;
	

	//revivalTransform_.scale = { 137.0f,22.0f,70.0f };
	//revivalTransform_.translate = { 571.0f,650.0f,1.0f };
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


	//影の計算
	shadowColor_.w = 1 - (player_->GetWorldPosition().y / PlayerConstants::kMaxShadowHeight);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { player_->GetWorldPosition().x,PlayerConstants::kShadowHeight,player_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	//キャラクターの更新
	player_->Update();

	spriteHpSize_.x = player_->GetHp();
	spriteMpSize_.x = player_->GetMp();
	if (spriteHpSize_.x >= 0) {
		spriteHP_->SetSize(spriteHpSize_);
	}
	if (spriteMpSize_.x > 0) {
		spriteMP_->SetSize(spriteMpSize_);
		
	}
	
	

	//HP,MP表示の計算
	if (spriteHpSize_.x < PlayerConstants::kPlayerMaxHP) {
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

	if (spriteMpSize_.x < PlayerConstants::kPlayerMaxMP) {
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


	if (spriteHpSize_.x < PlayerConstants::kPlayerHpLowThreshold) {
		hpColor_ = { 5.0f,0.0f,0.0f,1.0f };
		hpNumColor_ = { 5.0f,0.0f,0.0f,1.0f };

	}
	else if (spriteHpSize_.x <= PlayerConstants::kPlayerHpMediumThreshold) {
		hpNumColor_ = { 1.0f,0.2f,0.0f,1.0f };
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}
	else {
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
		hpNumColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

	/*/復活させる時の(今は使ってない)
	Revival();
	if (revivalTransform_.scale.x >= 120.0f) {
		revivalTransform_.scale.x = 120.0f;
	}
	spriteRevival_->SetSize(Vector2(revivalTransform_.scale.x, revivalTransform_.scale.y));
	*/
	

	ImGui::Begin("Sprite");
	ImGui::DragFloat2("Hpsize", &spriteHpSize_.x, 1.0f);
	ImGui::DragFloat2("Mpsize", &spriteMpSize_.x, 1.0f);
	ImGui::DragFloat4("HpColor", &hpColor_.x, 0.1f);
	ImGui::End();
};

void PlayerManager::Draw(const ViewProjection& camera) {
	
	player_->Draw(camera);
	particle_->Draw(camera);
	shadowModel_->Draw(worldTransformShadow_,camera,false);


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

	if (player_->GetHp() >= PlayerConstants::kPlayerMaxHP) {
		HPnum_[2]->Draw();
	}
	if (player_->GetMp() >= PlayerConstants::kPlayerMaxMP) {
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


void PlayerManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}


const WorldTransform& PlayerManager::GetWorldTransform() { return player_->GetWorldTransform(); }
Vector3 PlayerManager::katanaPos() { return player_->GetWorldTransformCollision().GetWorldPos(); }
void PlayerManager::SetViewProjection(const ViewProjection* viewProjection) {
	player_->SetViewProjection(viewProjection);
}