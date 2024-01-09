#include "PlayerManager.h"

void PlayerManager::Initialize() {

	Model_.reset(Model::CreateModelFromObj("resources/Player/float_Head.obj", "resources/Player/tex.png"));
	HammerModel_.reset(Model::CreateModelFromObj("resources/katana/katana.obj",  "resources/katana/kata.png"));
	
	spriteHP0_.reset(Sprite::Create("resources/Player/life1.png"));
	spriteHPG0_.reset(Sprite::Create("resources/Player/life0.png"));
	

	player_ = std::make_unique<Player>();
	player_->Initialize();

	
	HpTransform_.scale = {1.0f, 1.0f, 1.0f};
	HpTransform_.translate = {10.0f, 6.0f, 1.0f};
}

void PlayerManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	player_->Update();

	spriteHP0_->SetPosition({HpTransform_.translate.x, HpTransform_.translate.y});
	spriteHP0_->SetSize({HpTransform_.scale.x, HpTransform_.scale.y});
	spriteHPG0_->SetSize({0.5f, 0.08f});
	spriteHPG0_->SetPosition({HpTransform_.translate.x, HpTransform_.translate.y});


	ImGui::Begin("spritePos");
	ImGui::SliderFloat3("roto", &HpTransform_.translate.x, 500.0f, 700.0f);
	ImGui::End();
};

void PlayerManager::Draw(const ViewProjection& camera) {
	Model_->Draw(player_->GetWorldTransformHead(), camera, false);

	if (player_->IsAttack()) {
		HammerModel_->Draw(player_->GetWorldTransformHammer(), camera, false);

	}
	
};

void PlayerManager::DrawUI() {
	Transform uv;
	spriteHPG0_->Draw(uv);
	spriteHP0_->Draw(uv);
}

// 衝突を検出したら呼び出されるコールバック関数
void PlayerManager::OnCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		player_->OnCollision(worldTransform);
	}
	
};


const WorldTransform& PlayerManager::GetWorldTransform() { return player_->GetWorldTransform(); }
Vector3 PlayerManager::GetWorldPos() { return player_->GetWorldTransform().GetWorldPos(); }
Vector3 PlayerManager::katanaPos() { return player_->GetWorldTransformCollision().GetWorldPos(); }
void PlayerManager::SetViewProjection(const ViewProjection* viewProjection) {
	player_->SetViewProjection(viewProjection);
}