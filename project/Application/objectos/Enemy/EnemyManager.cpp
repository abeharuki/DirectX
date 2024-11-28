#include "EnemyManager.h"
#include <numbers>

void EnemyManager::Initialize() {
	
	rockModel_.reset(Sphere::CreateSphere("resources/skydome/sky.png"));

	nameModel_.reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/name.png"));

	damageNumModel_[playerNum].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/num/20.png"));
	damageNumModel_[healerNum].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/num/10.png"));
	damageNumModel_[renjuNum].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/num/20.png"));
	damageNumModel_[tankNum].reset(Model::CreateFromNoDepthObj("resources/particle/plane.obj", "resources/Enemy/num/10.png"));
	
	shadowModel_.reset(Model::CreateModelFromObj("resources/particle/plane.obj", "resources/particle/circle.png"));
	shadowModel_->SetBlendMode(BlendMode::kSubtract);

	enemy_ = std::make_unique<Enemy>();
	enemy_->Initialize();

	worldTransformName_.Initialize();
	worldTransformShadow_.Initialize();
	worldTransformShadow_.rotate = { -1.571f,0.0f,0.0f };
	worldTransformShadow_.scale = { 6.f,6.f,1.0f };

	worldTransformShadow_.translate = { enemy_->GetWorldPosition().x,0.1f,enemy_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	worldTransformNum_.resize(kdamageNumMax);
	for (int i = 0; i < kdamageNumMax; ++i) {
		worldTransformNum_[i].Initialize();
		worldTransformNum_[i].scale = { 1,1,1 };
	}

	HpTransform_.scale = { 800.0f, 40.0f, 1.0f };
	HpTransform_.translate = { 300.0f, 20.0f, 1.0f };

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

	//影の計算
	shadowColor_.w = 1 - (enemy_->GetWorldPosition().y / 3.9f);

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { enemy_->GetWorldPosition().x,0.09f,enemy_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	enemy_->SetHP(HpTransform_.scale.x);
	enemy_->SetDead(isDead_);
	enemy_->Update();

	//カメラとの距離の計算
	playerLength_ = Math::Length(Math::Subract(playerPos_, enemy_->GetWorldPosition()));
	//プレイヤーとの距離に応じて名前のサイズの変更
	nameScale_ = (playerLength_ / 17.0f) + (14.0f / 17.0f);
	worldTransformName_.scale = { nameScale_ ,nameScale_ , 1.f};

	//体力によって名前の色を変更
	if (HpTransform_.scale.x <= 200.0f) {
		color_ = { 1.0f,0.0f,0.0f,1.0f };
	}else if (HpTransform_.scale.x <= 400.0f) {
		color_ = { 1.0f,0.4f,0.0f,1.0f };
	}

	nameModel_->SetColor({ color_ });
	
	DamageNumMath();//ダメージ表示
	Billboard();//ビルボードの計算
	worldTransformName_.TransferMatrix();
	for (int i = 0; i < kdamageNumMax; ++i) {
		worldTransformNum_[i].TransferMatrix();
	}

	ImGui::Begin("EnemyManager");
	ImGui::Text("Length%f", playerLength_);
	ImGui::DragFloat("HpParameter", &HpTransform_.scale.x,1.f,0.0f);
	ImGui::DragFloat3("translat", &worldTransformName_.translate.x, 0.1f);
	ImGui::DragFloat3("rotate", &worldTransformName_.rotate.x, 0.1f);
	ImGui::DragFloat3("scale", &worldTransformName_.scale.x, 0.1f);
	ImGui::DragFloat4("color", &color_.x, 0.1f);
	ImGui::End();

};

void EnemyManager::Draw(const ViewProjection& camera) {
	enemy_->Draw(camera);
	rockModel_->Draw(enemy_->GetWorldTransformRock(), camera, false);
	shadowModel_->Draw(worldTransformShadow_, camera, false);
}
void EnemyManager::NoDepthDraw(const ViewProjection& camera){
	if (enemy_->GetBattle()) {
		nameModel_->Draw(worldTransformName_, camera, false);
	}
	
	for (int i = 0; i < kdamageNumMax; ++i) {
		damageNumModel_[i]->Draw(worldTransformNum_[i], camera, false);
	}
};

void EnemyManager::DrawUI() {
	
}

void EnemyManager::DamageNumMath(){
	damageNumModel_[playerNum]->SetColor({ 1.f,1.f,1.f,playerNumAlpha_ });
	damageNumModel_[healerNum]->SetColor({ 1.f,1.f,1.f,healerNumAlpha_ });
	damageNumModel_[renjuNum]->SetColor({ 1.f,1.f,1.f,renjuNumAlpha_ });
	damageNumModel_[tankNum]->SetColor({ 1.f,1.f,1.f,tankNumAlpha_ });

	if (playerNumAlpha_ > 0.0f) {
		playerNumAlpha_ -= 0.08f;
		worldTransformNum_[playerNum].translate =Math::Lerp(worldTransformNum_[playerNum].translate, {playerNumMove_}, 0.05f);
	}
	if (healerNumAlpha_ > 0.0f) {
		healerNumAlpha_ -= 0.08f;
		worldTransformNum_[healerNum].translate = Math::Lerp(worldTransformNum_[healerNum].translate, { healerNumMove_ }, 0.05f);
	}
	if (tankNumAlpha_ > 0.0f) {
		tankNumAlpha_ -= 0.08f;
		worldTransformNum_[tankNum].translate = Math::Lerp(worldTransformNum_[tankNum].translate, { tankNumMove_ }, 0.05f);
	}
	if (renjuNumAlpha_ > 0.0f) {
		renjuNumAlpha_ -= 0.08f;
		worldTransformNum_[renjuNum].translate = Math::Lerp(worldTransformNum_[renjuNum].translate, { renjuNumMove_ }, 0.05f);
	}

}

// 衝突を検出したら呼び出されるコールバック関数
void EnemyManager::OnCollision() {
	isHit_ = true;
	if (isHit_ != preHit_) {
		
		HpTransform_.scale.x -= 20.0f;
		playerNumAlpha_ = 2.0f;

		worldTransformNum_[playerNum].translate.x = enemy_->GetWorldPosition().x +  RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[playerNum].translate.z = enemy_->GetWorldPosition().z +  RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[playerNum].translate.y = RandomGenerator::GetRandomFloat(3.0f, 6.0f);
		playerNumMove_ = { worldTransformNum_[playerNum].translate.x ,worldTransformNum_[playerNum].translate.y + 2.0f,worldTransformNum_[playerNum].translate.z };

	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}

};

void EnemyManager::OnHealerCollision() {
	isHitH_ = true;
	if (isHitH_ != preHitH_) {
		HpTransform_.scale.x -= 10.0f;
		healerNumAlpha_ = 2.0f;
		worldTransformNum_[healerNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[healerNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[healerNum].translate.y = RandomGenerator::GetRandomFloat(3.0f, 6.0f);
		healerNumMove_ = { worldTransformNum_[healerNum].translate.x ,worldTransformNum_[healerNum].translate.y + 2.0f,worldTransformNum_[healerNum].translate.z };

	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};
void EnemyManager::OnTankCollision() {
	isHitT_ = true;
	if (isHitT_ != preHitT_) {
		HpTransform_.scale.x -= 10.0f;
		tankNumAlpha_ = 2.0f;
		worldTransformNum_[tankNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[tankNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[tankNum].translate.y = RandomGenerator::GetRandomFloat(3.0f, 6.0f);
		tankNumMove_ = { worldTransformNum_[tankNum].translate.x ,worldTransformNum_[tankNum].translate.y + 2.0f,worldTransformNum_[tankNum].translate.z };

	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
};
void EnemyManager::OnRenjuCollision(bool skill) {
	isHitR_ = true;
	if (isHitR_ != preHitR_) {
		if (skill) {
			HpTransform_.scale.x -= 30.0f;
			damageNumModel_[renjuNum]->SetTexture("Enemy/num/30.png");
		}
		else {
			HpTransform_.scale.x -= 20.0f;
			damageNumModel_[renjuNum]->SetTexture("Enemy/num/20.png");
		}
		
		renjuNumAlpha_ = 2.0f;
		worldTransformNum_[renjuNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[renjuNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-3.0f, 3.0f);
		worldTransformNum_[renjuNum].translate.y = RandomGenerator::GetRandomFloat(3.0f, 6.0f);
		renjuNumMove_ = { worldTransformNum_[renjuNum].translate.x ,worldTransformNum_[renjuNum].translate.y + 2.0f,worldTransformNum_[renjuNum].translate.z };

	}

	if (HpTransform_.scale.x <= 0) {
		HpTransform_.scale.x = 0;
	}
}
void EnemyManager::Billboard(){
	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(camera_.matView);
	billboardMatrix.m[3][0] = enemy_->GetWorldPosition().x;
	billboardMatrix.m[3][1] = 9.5f;
	billboardMatrix.m[3][2] = enemy_->GetWorldPosition().z;

	
	worldTransformName_.matWorld_ = Math::MakeScaleMatrix(worldTransformName_.scale) * billboardMatrix;


	for (int i = 0; i < kdamageNumMax; ++i) {
		
		Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(camera_.matView);
		billboardMatrixNum.m[3][0] = worldTransformNum_[i].translate.x;
		billboardMatrixNum.m[3][1] = worldTransformNum_[i].translate.y;
		billboardMatrixNum.m[3][2] = worldTransformNum_[i].translate.z;
		worldTransformNum_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformNum_[i].scale) * billboardMatrixNum;
	}
};

Vector3 EnemyManager::GetRockWorldPos() { return enemy_->GetWorldTransformRock().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }