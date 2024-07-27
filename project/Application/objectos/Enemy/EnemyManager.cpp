#include "EnemyManager.h"

void EnemyManager::Initialize() {

	rockModel_.reset(Sphere::CreateSphere("resources/skydome/sky.png"));

	spriteHP_.reset(Sprite::Create("resources/enemy/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/HPG.png"));
	spriteBoss_.reset(Sprite::Create("resources/enemy/BOSS.png"));

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();



	HpTransform_.scale = { 800.0f, 40.0f, 1.0f };
	HpTransform_.translate = { 270.0f, 20.0f, 1.0f };

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

	spriteHP_->SetPosition({ HpTransform_.translate.x, HpTransform_.translate.y });
	spriteHP_->SetSize({ HpTransform_.scale.x, HpTransform_.scale.y });
	spriteHPG_->SetSize({ 800.0f, 40.0f });
	spriteHPG_->SetPosition({ HpTransform_.translate.x, HpTransform_.translate.y });

	//spriteBoss_->SetSize({ 1.5f, 1.5f });
	spriteBoss_->SetPosition({ HpTransform_.translate.x, HpTransform_.translate.y + 30 });

	if (HpTransform_.scale.x <= 0) {
		isDead_ = true;
	}
	enemy_->isDead(isDead_);
	enemy_->Update();



};

void EnemyManager::Draw(const ViewProjection& camera) {
	//Model_->Draw(enemy_->GetWorldTransformBody(), camera, false);
	enemy_->Draw(camera);
	rockModel_->Draw(enemy_->GetWorldTransformRock(), camera, false);
};

void EnemyManager::DrawUI() {
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteBoss_->Draw();
}

// 衝突を検出したら呼び出されるコールバック関数
void EnemyManager::OnCollision() {
	isHit_ = true;
	if (isHit_ != preHit_) {
		HpTransform_.scale.x -= 30.0f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}

};

void EnemyManager::OnHealerCollision() {
	isHitH_ = true;
	if (isHitH_ != preHitH_) {
		HpTransform_.scale.x -= 10.01f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};
void EnemyManager::OnTankCollision() {
	isHitT_ = true;
	if (isHitT_ != preHitT_) {
		HpTransform_.scale.x -= 20.02f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};
void EnemyManager::OnRenjuCollision() {
	isHitR_ = true;
	if (isHitR_ != preHitR_) {
		HpTransform_.scale.x -= 20.02f;
	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};

Vector3 EnemyManager::GetRockWorldPos() { return enemy_->GetWorldTransformRock().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }