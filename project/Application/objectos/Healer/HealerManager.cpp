#include "HealerManager.h"

void HealerManager::Initialize() {
	

	spriteHP_.reset(Sprite::Create("resources/Player/HP.png"));
	spriteHPG_.reset(Sprite::Create("resources/HPG.png"));
	spriteMP_.reset(Sprite::Create("resources/Player/MP.png"));
	spriteMPG_.reset(Sprite::Create("resources/HPG.png"));
	sprite4P_.reset(Sprite::Create("resources/4P.png"));
	spriteH_.reset(Sprite::Create("resources/H.png"));
	spriteM_.reset(Sprite::Create("resources/M.png"));
	spriteName_.reset(Sprite::Create("resources/healer.png"));
	//CaneModel_.reset(Model::CreateModelFromObj("resources/cane/cane.obj", "resources/cane/cane.png"));

	healer_ = std::make_unique<Healer>();
	healer_->Initialize();

	spriteHP_->SetPosition(Vector2{ 1106.0f,615.0f });
	spriteHPG_->SetPosition(Vector2{ 1106.0f,615.0f });
	spriteMP_->SetPosition(Vector2{ 1106.0f,640.0f });
	spriteMPG_->SetPosition(Vector2{ 1106.0f,640.0f });
	sprite4P_->SetPosition(Vector2{ 995.0f,583.0f });
	spriteH_->SetPosition(Vector2{ 1097.0f,593.0f });
	spriteM_->SetPosition(Vector2{ 1097.0f,618.0f });
	spriteName_->SetPosition(Vector2{ 995.0f,573.0f });

	spriteHPG_->SetSize(Vector2{ 100.0f,10.0f });
	spriteMPG_->SetSize(Vector2{ 100.0f,10.0f });
	sprite4P_->SetSize(Vector2{ 93.0f,85.0f });
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

}

void HealerManager::Update() {
	// 前のフレームの当たり判定のフラグを取得
	preHit_ = isHit_;
	isHit_ = false;
	// 敵の判定
	preHitE_ = isHitE_;
	isHitE_ = false;
	particle_->SetEmitter(emitter_);
	
	if (isParticle_) {
		particle_->Update();
		if (!particle_->GetEmit()) {
			isParticle_ = false;
		}
	}
	
	

	healer_->Update();
	healer_->followPlayer(playerPos_);

	spriteHpSize_.x = healer_->GetHp();
	spriteMpSize_.x = healer_->GetMp();
	spriteHP_->SetSize(spriteHpSize_);
	spriteMP_->SetSize(spriteMpSize_);

	spriteHP_->SetColor(hpColor_);
	if (spriteHpSize_.x < 20) {
		hpColor_ = { 5.0f,0.0f,0.0f,1.0f };
	}
	else {
		hpColor_ = { 1.0f,1.0f,1.0f,1.0f };
	}

};

void HealerManager::Draw(const ViewProjection& camera) {
	healer_->Draw(camera);
	particle_->Draw(camera);
	if (healer_->IsAttack()) {
		//CaneModel_->Draw(healer_->GetWorldTransformCane(), camera, false);

	}

}


void HealerManager::DrawUI(){
	spriteHPG_->Draw();
	spriteHP_->Draw();
	spriteMPG_->Draw();
	spriteMP_->Draw();
	sprite4P_->Draw();
	spriteH_->Draw();
	spriteM_->Draw();
	spriteName_->Draw();
};

void HealerManager::RenderDirect(const ViewProjection& camera) {
	
};

void HealerManager::followPlayer(Vector3 playerPos) { playerPos_ = playerPos; }

void HealerManager::OnAllyCollision(const WorldTransform& worldTransform) {
	isHit_ = true;
	if (isHit_ != preHit_) {
		healer_->OnAllyCollision(worldTransform);
	}


}



void HealerManager::SetParticlePos(Vector3 pos) {
	emitter_.translate = pos;
	isParticle_ = true;
}



const WorldTransform& HealerManager::GetWorldTransform() { return healer_->GetWorldTransform(); }
void HealerManager::SetViewProjection(const ViewProjection& viewProjection) {
	healer_->SetViewProjection(viewProjection);
}