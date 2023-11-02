#include "Player.h"
#include <imgui.h>


void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// 初期化
	worldTransform_.Initialize();
#ifdef _DEBUG
	GlobalVariables* globalVariables = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	// グループを追加
	GlobalVariables::GetInstance()->CreateGroup(groupName);

	globalVariables->AddItem(groupName, "DashSpeed", kDashSpeed);
	//globalVariables->AddItem(groupName, "behaviorDashTime", behaviorDashTime);

	
	
#endif
}

void Player::Update(){

	BaseCharacter::Update();
	Move();
	

	worldTransform_.UpdateMatrix();
	
	ImGui::Begin("P");
	ImGui::DragFloat4("translation", &worldTransform_.translate.x, 0.01f);
	ImGui::Text(" X%f Y%f Z%f", worldTransform_.matWorld_.m[3][0], worldTransform_.matWorld_.m[3][1],worldTransform_.matWorld_.m[3][2]);
	ImGui::Text(" isHit%d", isHit_);
	ImGui::Text(" isHitFloor%d", isHitFloor_);
	ImGui::Text(" jump%d", jump_);
	ImGui::End();

	ApplyGlobalVariables();
}

void Player::Draw(const ViewProjection& viewprojection){
	models_[0]->Draw(worldTransform_, viewprojection);
}

Player::Player(){
	upSpeed_ = 0.0f;
	fallSpeed_ = 0.0f;
	jump_ = false;
	isHit_ = false;
	isHitFloor_ = false;
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

Vector3 Player::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransform_.translate.x;
	worldPos.y = worldTransform_.translate.y;
	worldPos.z = worldTransform_.translate.z;
	return worldPos;
}

void Player::SetPosition(Vector3 Pos) { worldTransform_.translate = Pos; }

void Player::joyMove() {
	// ゲームパッドの状態を得る変数(XINPUT)
	XINPUT_STATE joyState;
		// ジョイスティックの状態取得
	if (KeyInput::GetInstance()->GetJoystickState(0, joyState)) {
			const float value = 0.7f;
			bool isMove = false;

			// 移動速度
			const float kCharacterSpeed = 0.2f;
			// 移動量
			Vector3 move = {
			    (float)joyState.Gamepad.sThumbLX / SHRT_MAX, 0.0f,
			    (float)joyState.Gamepad.sThumbLY / SHRT_MAX};

			if (Math::Length(move) > value) {
				isMove = true;
			}

			move = Math::Multiply(kCharacterSpeed, move);

			Matrix4x4 rotateMatrix = Math::Multiply(
			    Math::MakeRotateXMatrix(viewProjection_.rotation_.x),
			    Math::Multiply(
			        Math::MakeRotateYMatrix(viewProjection_.rotation_.y),
			        Math::MakeRotateZMatrix(viewProjection_.rotation_.z)));
			// move = utility_->Normalize(move);
			move = Math::TransformNormal(move, rotateMatrix);

			if (isMove) {
				
				worldTransform_.translate =
				    Math::Add(worldTransform_.translate, move);
				destinationAngleY_ = std::atan2(move.x, move.z);
			}

			worldTransform_.rotate.y =
			    Math::LerpShortAngle(worldTransform_.rotate.y, destinationAngleY_, 0.2f);
	}

		

		// ダッシュボタンを押したら
	    if (behaviorRequest_ == Behavior::kRoot) {
			if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
				behaviorRequest_ = Behavior::kDash;
			}
		}
	
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
	
	if (isHit_ || isHitFloor_) {
		fallSpeed_ = 0.0f;
		upSpeed_ = 0.0f;
		jump_ = false;
		if (worldTransform_.translate.y < 0) {
			worldTransform_.translate.y = 0;  
		}

	}

	if (!isHit_ && !isHitFloor_) {
		IsFall();
	} 

	if (worldTransform_.translate.y < -150) {
		fallSpeed_ = 0.0f;
		worldTransform_.translate = {0.0f, 0.0f, 0.0f};
	}
	

}

void Player::DashUpdate() {
	//dashTimer -= 4;
	
	Vector3 velocity = {0, 0, kDashSpeed};

	velocity = Math::TransformNormal(velocity, worldTransform_.matWorld_);
	worldTransform_.translate = Math::Add(worldTransform_.translate, velocity);

	

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Relationship(const WorldTransform& worldTransformFloor) {

	if (!worldTransform_.parent_) {

		// 移動床のワールド座標
		Vector3 objectWorldPos = {
		    worldTransformFloor.matWorld_.m[3][0], worldTransformFloor.matWorld_.m[3][1],
		    worldTransformFloor.matWorld_.m[3][2]};



		// プレイヤーローカル座標
		Matrix4x4 localPos =
		    Math::Multiply(Math::Inverse(worldTransformFloor.matWorld_), worldTransform_.matWorld_);

		Vector3 playerLocalPos = {localPos.m[3][0], localPos.m[3][1]+1.0f, localPos.m[3][2]};

		
		
		//ローカル座標系に入れる
		//worldTransform_.matWorld_ = localPos;
		worldTransform_.translate = playerLocalPos;

		Setparent(&worldTransformFloor);

		ImGui::Begin("P");
		
		
		ImGui::End();
	}
	
}

void Player::Setparent(const WorldTransform* parent) { 
	worldTransform_.parent_ = parent; 
	

};

void Player::DeleteParent() {
	if (worldTransform_.parent_) {
		worldTransform_.translate = worldTransform_.GetWorldPos();
		worldTransform_.parent_ = nullptr;
	}
}

void Player::IsFall() {

	fallSpeed_ -= 0.01f;
	upSpeed_ += fallSpeed_;
	worldTransform_.translate.y += upSpeed_;
}

void Player::OnCollision() {isHit_ = true;}

void Player::OutCollision() { isHit_ = false; }

void Player::OnCollisionFloor() { isHitFloor_ = true; }

void Player::OutCollisionFloor() { isHitFloor_ = false; }


void Player::ApplyGlobalVariables() {
#ifdef _DEBUG

	GlobalVariables* globalVaribles = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	kDashSpeed = globalVaribles->GetFloatValue(groupName, "DashSpeed");
	
	
#endif
}