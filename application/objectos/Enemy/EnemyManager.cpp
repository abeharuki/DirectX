#include "EnemyManager.h"

void EnemyManager::Initialize() {

	Model_.reset( Model::CreateModelFromObj("resources/enemy/enemy.obj", "resources/enemy/enemy.png"));
	bulletModel_.reset( Model::CreateModelFromObj("resources/Renju/cube.obj", "resources/Renju/Bullet.png"));
	
	spriteHP_.reset(Sprite::Create("resources/enemy/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/HPG.png"));


	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	worldTransformBase_.Initialize();
	worldTransform_.Initialize();
	worldTransform_ = worldTransformBase_;

	HpTransform_.scale = {2.0f, 0.08f, 1.0f};
	HpTransform_.translate = {270.0f, 100.0f, 1.0f};

}

void EnemyManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	preHitH_ = isHitH_;
	isHitH_ = false;
	preHitR_ = isHitR_;
	isHitR_ = false;
	preHitT_ = isHitT_;
	isHitT_ = false;

	spriteHP_->SetPosition({HpTransform_.translate.x, HpTransform_.translate.y});
	spriteHP_->SetSize({HpTransform_.scale.x, HpTransform_.scale.y});
	spriteHPG_->SetSize({2.0f, 0.08f});
	spriteHPG_->SetPosition({HpTransform_.translate.x, HpTransform_.translate.y});

	if (spriteHP_ <= 0) {
		isDead_ = true;
	}
	if (isDead_) {
		enemy_->isDead(isDead_);
	}

	enemy_->Update(); 
	worldTransform_.UpdateMatrix();

	ImGui::Begin("enemy");
	ImGui::SliderFloat3("size", &HpTransform_.scale.x, 0.0f,1.0f);
	ImGui::SliderFloat3("pos", &HpTransform_.translate.x, 0.0f, 500.0f);
	ImGui::End();
};

void EnemyManager::Draw(const ViewProjection& camera) {
	Model_->Draw(enemy_->GetWorldTransform(), camera, false);
};

void EnemyManager::DrawUI() { 
	Transform uv;
	spriteHPG_->Draw(uv);
	spriteHP_->Draw(uv);
}

// 衝突を検出したら呼び出されるコールバック関数
void EnemyManager::OnCollision(){ 
	isHit_ = true;
	if (isHit_ != preHit_) {
		HpTransform_.scale.x-=0.03f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}

};

void EnemyManager::OnHealerCollision(){
	isHitH_ = true;
	if (isHitH_ != preHitH_) {
		HpTransform_.scale.x -=0.01f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};
void EnemyManager::OnTankCollision(){
	isHitT_ = true;
	if (isHitT_ != preHitT_) {
		HpTransform_.scale.x -= 0.02f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};
void EnemyManager::OnRenjuCollision(){
	isHitR_ = true;
	if (isHitR_ != preHitR_) {
		HpTransform_.scale.x -= 0.02f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};

Vector3 EnemyManager::GetWorldPos() { return enemy_->GetWorldTransform().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }