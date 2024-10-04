#include "EnemyManager.h"
#include <numbers>

void EnemyManager::Initialize() {

	rockModel_.reset(Sphere::CreateSphere("resources/skydome/sky.png"));

	nameModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/name.png"));

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	worldTransformName_.Initialize();
	worldTransformName_.scale = { 2.5f,2.5f,1.f };

	HpTransform_.scale = { 800.0f, 40.0f, 1.0f };
	HpTransform_.translate = { 270.0f, 20.0f, 1.0f };

	color_ = { 1.0f,1.0f,1.0f,1.0f };
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

	if (HpTransform_.scale.x <= 0) {
		isDead_ = true;
	}
	enemy_->isDead(isDead_);
	enemy_->Update();

	if (HpTransform_.scale.x <= 200.0f) {
		color_ = { 1.0f,0.0f,0.0f,1.0f };
	}else if (HpTransform_.scale.x <= 400.0f) {
		color_ = { 1.0f,0.4f,0.0f,1.0f };
	}

	nameModel_->SetColor({ color_ });

	/*worldTransformName_.translate.x = enemy_->GetWorldPosition().x;
	worldTransformName_.translate.z = enemy_->GetWorldPosition().z;*/
	Billboard();
	worldTransformName_.TransferMatrix();

	ImGui::Begin("EnemyManager");
	ImGui::Text("HP%f", HpTransform_.translate.x / 8.0f);
	ImGui::DragFloat("HpParameter", &HpTransform_.scale.x,1.f,0.0f);
	ImGui::DragFloat3("translat", &worldTransformName_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransformName_.rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransformName_.scale.x, 0.1f);
	ImGui::DragFloat4("color", &color_.x, 0.1f);
	ImGui::End();

};

void EnemyManager::Draw(const ViewProjection& camera) {
	//Model_->Draw(enemy_->GetWorldTransformBody(), camera, false);
	

	enemy_->Draw(camera);
	rockModel_->Draw(enemy_->GetWorldTransformRock(), camera, false);
	nameModel_->Draw(worldTransformName_, camera, false);
};

void EnemyManager::DrawUI() {
	
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
}
void EnemyManager::Billboard(){
	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(camera_.matView);
	billboardMatrix.m[3][0] = enemy_->GetWorldPosition().x;
	billboardMatrix.m[3][1] = 5.0f;
	billboardMatrix.m[3][2] = enemy_->GetWorldPosition().z;

	
	worldTransformName_.matWorld_ = Math::MakeScaleMatrix(worldTransformName_.scale) * billboardMatrix;


};

Vector3 EnemyManager::GetRockWorldPos() { return enemy_->GetWorldTransformRock().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }