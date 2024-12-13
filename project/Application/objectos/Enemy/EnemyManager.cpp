#include "EnemyManager.h"

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
	worldTransformShadow_.rotate = EnemyConstants::kShadowRotate;
	worldTransformShadow_.scale = EnemyConstants::kShadowScale;

	worldTransformShadow_.translate = { enemy_->GetWorldPosition().x,0.1f,enemy_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	worldTransformNum_.resize(kDamageNumMax);
	for (int i = 0; i < kDamageNumMax; ++i) {
		worldTransformNum_[i].Initialize();
		worldTransformNum_[i].scale = { 1,1,1 };
	}

	hp_ = EnemyConstants::kMaxHp;

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

	if (hp_ <= 0) {
		isDead_ = true;
	}

	//影の計算
	shadowColor_.w = 1 - (enemy_->GetWorldPosition().y / EnemyConstants::kMaxShadowHeight);//高さによってアルファ値を変える

	shadowModel_->SetColor(shadowColor_);
	worldTransformShadow_.translate = { enemy_->GetWorldPosition().x,0.09f,enemy_->GetWorldPosition().z };
	worldTransformShadow_.UpdateMatrix();

	enemy_->SetHP(hp_);
	enemy_->SetDead(isDead_);
	enemy_->Update();

	//カメラとの距離の計算
	playerLength_ = Math::Length(Math::Subract(playerPos_, enemy_->GetWorldPosition()));
	//プレイヤーとの距離に応じて名前のサイズの変更
	nameScale_ = (playerLength_ / EnemyConstants::kDistanceScaleBase) + (EnemyConstants::kDistanceScaleOffset / EnemyConstants::kDistanceScaleBase);
	worldTransformName_.scale = { nameScale_ ,nameScale_ , 1.f};

	//体力によって名前の色を変更
	if (hp_ <= EnemyConstants::kLowHpThreshold) {
		color_ = { 1.0f,0.0f,0.0f,1.0f };
	}else if (hp_ <= EnemyConstants::kMidHpThreshold) {
		color_ = { 1.0f,0.4f,0.0f,1.0f };
	}

	nameModel_->SetColor({ color_ });
	
	DamageNumMath();//ダメージ表示
	Billboard();//ビルボードの計算
	worldTransformName_.TransferMatrix();
	for (int i = 0; i < kDamageNumMax; ++i) {
		worldTransformNum_[i].TransferMatrix();
	}

	ImGui::Begin("EnemyManager");
	ImGui::Text("Length%f", playerLength_);
	ImGui::DragFloat("HpParameter", &hp_,1.f,0.0f);
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
	
	for (int i = 0; i < kDamageNumMax; ++i) {
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

	if (playerNumAlpha_ > EnemyConstants::kZeroAlpha) {
		playerNumAlpha_ -= EnemyConstants::kAlphaDecreaseRate;
		worldTransformNum_[playerNum].translate =Math::Lerp(worldTransformNum_[playerNum].translate, {playerNumMove_}, 0.05f);
	}
	if (healerNumAlpha_ > EnemyConstants::kZeroAlpha) {
		healerNumAlpha_ -= EnemyConstants::kAlphaDecreaseRate;
		worldTransformNum_[healerNum].translate = Math::Lerp(worldTransformNum_[healerNum].translate, { healerNumMove_ }, 0.05f);
	}
	if (tankNumAlpha_ > EnemyConstants::kZeroAlpha) {
		tankNumAlpha_ -= EnemyConstants::kAlphaDecreaseRate;
		worldTransformNum_[tankNum].translate = Math::Lerp(worldTransformNum_[tankNum].translate, { tankNumMove_ }, 0.05f);
	}
	if (renjuNumAlpha_ > EnemyConstants::kZeroAlpha) {
		renjuNumAlpha_ -= EnemyConstants::kAlphaDecreaseRate;
		worldTransformNum_[renjuNum].translate = Math::Lerp(worldTransformNum_[renjuNum].translate, { renjuNumMove_ }, 0.05f);
	}

}

// 衝突を検出したら呼び出されるコールバック関数
void EnemyManager::OnCollision() {
	isHit_ = true;

	if (isHit_ != preHit_) {
		
		if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->GetThreshold() <= EnemyConstants::kBarrierThresholdLimit) {
			damageNumModel_[playerNum]->SetTexture("Enemy/num/0.png");
		}
		else {
			damageNumModel_[playerNum]->SetTexture("Enemy/num/20.png");

			hp_ -= EnemyConstants::kDamagePlayer;
			
		}

		playerNumAlpha_ = EnemyConstants::kInitAlpha;

		worldTransformNum_[playerNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[playerNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[playerNum].translate.y = RandomGenerator::GetRandomFloat(EnemyConstants::kRandomHeightMin, EnemyConstants::kRandomHeightMax);
		playerNumMove_ = { worldTransformNum_[playerNum].translate.x ,worldTransformNum_[playerNum].translate.y + EnemyConstants::kYPositionOffset,worldTransformNum_[playerNum].translate.z };

		
	}

	if (hp_ <= 0) {
		hp_ = 0;
	}

};

void EnemyManager::OnHealerCollision() {
	isHitH_ = true;
	

	if (isHitH_ != preHitH_) {
		if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->GetThreshold() <= EnemyConstants::kBarrierThresholdLimit) {
			damageNumModel_[healerNum]->SetTexture("Enemy/num/0.png");

		}
		else {
			damageNumModel_[healerNum]->SetTexture("Enemy/num/10.png");

			hp_ -= EnemyConstants::kDamageHealer;
			
		}
		healerNumAlpha_ = EnemyConstants::kInitAlpha;
		worldTransformNum_[healerNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[healerNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[healerNum].translate.y = RandomGenerator::GetRandomFloat(EnemyConstants::kRandomHeightMin, EnemyConstants::kRandomHeightMax);
		healerNumMove_ = { worldTransformNum_[healerNum].translate.x ,worldTransformNum_[healerNum].translate.y + EnemyConstants::kYPositionOffset,worldTransformNum_[healerNum].translate.z };

	}

	if (hp_ <= 0) {
		hp_ = 0;
	}
};
void EnemyManager::OnTankCollision() {
	isHitT_ = true;

	

	if (isHitT_ != preHitT_) {
		if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->GetThreshold() <= EnemyConstants::kBarrierThresholdLimit) {
			damageNumModel_[tankNum]->SetTexture("Enemy/num/0.png");
		}
		else {
			damageNumModel_[tankNum]->SetTexture("Enemy/num/10.png");

			hp_ -= EnemyConstants::kDamageTank;
		}
		tankNumAlpha_ = EnemyConstants::kInitAlpha;
		worldTransformNum_[tankNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[tankNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[tankNum].translate.y = RandomGenerator::GetRandomFloat(EnemyConstants::kRandomHeightMin, EnemyConstants::kRandomHeightMax);
		tankNumMove_ = { worldTransformNum_[tankNum].translate.x ,worldTransformNum_[tankNum].translate.y + EnemyConstants::kYPositionOffset,worldTransformNum_[tankNum].translate.z };

	}

	if (hp_ <= 0) {
		hp_ = 0;
	}
};
void EnemyManager::OnRenjuCollision(bool skill) {
	isHitR_ = true;
	if (isHitR_ != preHitR_) {
		if (skill) {
			hp_ -= EnemyConstants::kDamageRenjuSkill;
			damageNumModel_[renjuNum]->SetTexture("Enemy/num/30.png");
		}
		else {
			if (enemy_->GetBehavior() == Behavior::kAttack && enemy_->GetBehaviorAttack() == BehaviorAttack::kHenchman && enemy_->GetThreshold() <= EnemyConstants::kBarrierThresholdLimit) {
				damageNumModel_[renjuNum]->SetTexture("Enemy/num/0.png");
			}
			else {
				damageNumModel_[renjuNum]->SetTexture("Enemy/num/20.png");
				hp_ -= EnemyConstants::kDamageRenjuNormal;
				
			}

			
		}
		
		renjuNumAlpha_ = EnemyConstants::kInitAlpha;
		worldTransformNum_[renjuNum].translate.x = enemy_->GetWorldPosition().x + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[renjuNum].translate.z = enemy_->GetWorldPosition().z + RandomGenerator::GetRandomFloat(-EnemyConstants::kRandomOffsetRange, EnemyConstants::kRandomOffsetRange);
		worldTransformNum_[renjuNum].translate.y = RandomGenerator::GetRandomFloat(EnemyConstants::kRandomHeightMin, EnemyConstants::kRandomHeightMax);
		renjuNumMove_ = { worldTransformNum_[renjuNum].translate.x ,worldTransformNum_[renjuNum].translate.y + EnemyConstants::kYPositionOffset,worldTransformNum_[renjuNum].translate.z };

	}

	if (hp_ <= 0) {
		hp_ = 0;
	}
}
void EnemyManager::Billboard(){
	Matrix4x4 backToFrontMatrix = Math::MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = backToFrontMatrix * Math::Inverse(camera_.matView);
	billboardMatrix.m[3][0] = enemy_->GetWorldPosition().x;
	billboardMatrix.m[3][1] = EnemyConstants::kBillboardHeight;
	billboardMatrix.m[3][2] = enemy_->GetWorldPosition().z;

	
	worldTransformName_.matWorld_ = Math::MakeScaleMatrix(worldTransformName_.scale) * billboardMatrix;


	for (int i = 0; i < kDamageNumMax; ++i) {
		
		Matrix4x4 billboardMatrixNum = backToFrontMatrix * Math::Inverse(camera_.matView);
		billboardMatrixNum.m[3][0] = worldTransformNum_[i].translate.x;
		billboardMatrixNum.m[3][1] = worldTransformNum_[i].translate.y;
		billboardMatrixNum.m[3][2] = worldTransformNum_[i].translate.z;
		worldTransformNum_[i].matWorld_ = Math::MakeScaleMatrix(worldTransformNum_[i].scale) * billboardMatrixNum;
	}
};

Vector3 EnemyManager::GetRockWorldPos() { return enemy_->GetWorldTransformRock().GetWorldPos(); }

const WorldTransform& EnemyManager::GetWorldTransform() { return enemy_->GetWorldTransform(); }