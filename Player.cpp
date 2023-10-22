#include "Player.h"
#include <imgui.h>

void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// 初期化
	worldTransform_.Initialize();
	

	
}

void Player::Update(){

	BaseCharacter::Update();
	Move();

	worldTransform_.UpdateMatrix();

	ImGui::Begin("Player");
	ImGui::DragFloat4("translation", &worldTransform_.translate.x, 0.01f);
	ImGui::Text(" isHit%d", isHit_);
	ImGui::Text(" jump%d", jump_);
	ImGui::End();
}

void Player::Draw(const ViewProjection& viewprojection){
	models_[0]->Draw(worldTransform_, viewprojection);
}

Player::Player(){
	upSpeed_ = 0.0f;
	fallSpeed_ = 0.0f;
	jump_ = false;
	isHit_ = false;
};
Player::~Player(){};

Vector3 Player::GetWorldPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.matWorld_.m[3][0];
	worldPos.y = worldTransform_.matWorld_.m[3][1];
	worldPos.z = worldTransform_.matWorld_.m[3][2];
	return worldPos;
}


void Player::Move() {

	/*----------移動処理----------*/
	if (KeyInput::GetKey(DIK_D)) {
		worldTransform_.translate.x += 0.5f;

	}else if (KeyInput::GetKey(DIK_A)) {
		worldTransform_.translate.x -= 0.5f;
	}
	if (KeyInput::GetKey(DIK_W)) {
		worldTransform_.translate.z += 0.5f;

	} else if (KeyInput::GetKey(DIK_S)) {
		worldTransform_.translate.z -= 0.5f;
	}

	if (KeyInput::GetKey(DIK_UPARROW)) {
		worldTransform_.translate.y += 0.3f;

	} else if (KeyInput::GetKey(DIK_DOWNARROW)) {
		worldTransform_.translate.y -= 0.3f;
	}

	/*----------ジャンプ処理 & 落下処理----------*/
	if (KeyInput::GetKey(DIK_SPACE) && !jump_) {
		
		upSpeed_ = 1.0f;
		jump_ = true;
	}
	if (jump_) {
		worldTransform_.translate.y += upSpeed_;
	}
	
	if (isHit_ ) {
		fallSpeed_ = 0.0f;
		upSpeed_ = 0.0f;
		jump_ = false;
		if (worldTransform_.translate.y < 0) {
			worldTransform_.translate.y = 0;  
		}

	} else {
		IsFall();
	} 

	if (worldTransform_.translate.y < -150) {
		fallSpeed_ = 0.0f;
		worldTransform_.translate = {0.0f, 0.0f, 0.0f};
	}
	

}

void Player::IsFall() {

	fallSpeed_ -= 0.01f;
	upSpeed_ += fallSpeed_;
	worldTransform_.translate.y += upSpeed_;
}

void Player::OnCollision() {isHit_ = true;}

void Player::OutCollision() { isHit_ = false; }