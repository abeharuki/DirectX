#include "PlayerManager.h"


void PlayerManager::Initialize() {

	
	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("./resources/AnimatedCube", "tex.png", "bound3.gltf"));


	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	HammerModel_.reset(Model::CreateModelFromObj("resources/katana/katana.obj", "resources/katana/kata.png"));

	spriteRevival_.reset(Sprite::Create("resources/enemy/HP.png"));
	spriteRevivalG_.reset(Sprite::Create("resources/HPG.png"));

	for (int i = 0; i < 6; i++) {
		spriteHP_[i].reset(Sprite::Create("resources/Player/life1.png"));
		spriteHPG_[i].reset(Sprite::Create("resources/Player/life0.png"));
		spriteHP_[i]->SetAnchorPoint({ 0.5f,0.5f });
		spriteHPG_[i]->SetAnchorPoint({ 0.5f,0.5f });
	}

	player_ = std::make_unique<Player>();
	player_->Initialize();

	//worldTransformHead_.Initialize();
	worldTransformHead_ = player_->GetWorldTransform();

	emitter_.transform = {
		{0.8f, 0.8f, 0.8f},
		{0.0f, 0.0f, 0.0f},
		{0.0f, 0.0f, 0.0f}
	};
	emitter_.count = 10;
	emitter_.frequencyTime = 0;

	particle_.reset(ParticleSystem::Create("resources/particle/circle.png"));
	isParticle_ = false;
	HpTransform_.scale = { 70.0f, 70.0f, 70.0f };
	HpTransform_.translate = { 690.0f, 600.0f, 1.0f };

	revivalTransform_.scale = { 137.0f,22.0f,70.0f };
	revivalTransform_.translate = { 571.0f,650.0f,1.0f };
	spriteRevivalG_->SetSize(Vector2(120.0f, revivalTransform_.scale.y));
	spriteRevivalG_->SetPosition(Vector2(revivalTransform_.translate.x, revivalTransform_.translate.y));
	spriteRevival_->SetPosition(Vector2(revivalTransform_.translate.x, revivalTransform_.translate.y));
	hitCount_ = 6;
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

	for (int i = 0; i < 6; i++) {
		spriteHP_[i]->SetSize({ HpTransform_.scale.x, HpTransform_.scale.y });
		spriteHPG_[i]->SetSize({ HpTransform_.scale.x, HpTransform_.scale.y });
	}
	for (int i = 0; i < 6; i++) {
		spriteHP_[i]->SetPosition({ HpTransform_.translate.x - 20 * i, HpTransform_.translate.y });
		spriteHPG_[i]->SetPosition({ HpTransform_.translate.x - 20 * i, HpTransform_.translate.y });
	}

	for (int i = 6; i > -1; i--) {
		if (i < hitCount_) {
			break;
		}
		spriteHP_[i]->SetColor({ 0.0f, 0.0f, 0.0f, 0.0f });

	}

	if (hitCount_ <= 0) {
		//isDead_ = true;
	}
	if (Input::PushKey(DIK_P)) {
		isDead_ = true;
	}

	if (isParticle_) {
		particle_->Update();
		isParticle_ = false;
	}
	else {
		particle_->StopParticles();
	}



	player_->Update();

	hitCount_ = player_->HitCount();
	

	Revival();
	if (revivalTransform_.scale.x >= 120.0f) {
		revivalTransform_.scale.x = 120.0f;
	}
	spriteRevival_->SetSize(Vector2(revivalTransform_.scale.x, revivalTransform_.scale.y));

	ImGui::Begin("Sprite");
	ImGui::DragFloat3("size", &revivalTransform_.scale.x, 1.0f);
	ImGui::DragFloat3("pos", &revivalTransform_.translate.x, 1.0f);
	ImGui::End();

};

void PlayerManager::Draw(const ViewProjection& camera) {
	//Model_->Draw(player_->GetWorldTransformHead(), camera, false);
	player_->Draw(camera);
	
	particle_->Draw(camera);
	if (player_->IsAttack()) {
		HammerModel_->Draw(player_->GetWorldTransformHammer(), camera, false);

	}

};

void PlayerManager::DrawUI() {
	Transform uv;
	uv.scale = { 0.0f, 0.0f, 0.0f };
	uv.rotate = { 0.0f, 0.0f, 0.0f };
	uv.translate = { 0.0f, 0.0f, 0.0f };
	for (int i = 0; i < 6; i++) {
		spriteHPG_[i]->Draw();
		spriteHP_[i]->Draw();
	}

	if (player_->GetIsDead()) {
		spriteRevivalG_->Draw();
		spriteRevival_->Draw();

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
	if (player_->IsHit()) {
		//--hitCount_;
	}
};

void PlayerManager::SetParticlePos(Vector3 pos) {
	particle_->SetTranslate(pos);
	isParticle_ = true;
}


const WorldTransform& PlayerManager::GetWorldTransform() { return player_->GetWorldTransform(); }
Vector3 PlayerManager::katanaPos() { return player_->GetWorldTransformCollision().GetWorldPos(); }
void PlayerManager::SetViewProjection(const ViewProjection* viewProjection) {
	player_->SetViewProjection(viewProjection);
}