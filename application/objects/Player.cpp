#include "Player.h"
#include <imgui.h>


void Player::Initialize(const std::vector<Model*>& models) {
	// 基底クラスの初期化
	BaseCharacter::Initialize(models);

	// 初期化
	worldTransformBase_.Initialize();
	worldTransformHammer_.Initialize();
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

	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			
			break;
		case Behavior::kAttack:
			AttackInitialize();
			break;
		case Behavior::kDash:
			DashInitialize();
			break;
		case Behavior::kDead:
			
			break;
		}

		// 振る舞いリセット
		behaviorRequest_ = std::nullopt;
	}

	

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動
		Move();
		joyMove();
		break;
	case Behavior::kAttack:
		// 攻撃
		AttackUpdata();
		break;
	case Behavior::kDash:
		DashUpdata();
		break;
	case Behavior::kDead:
		
		break;
	}

	BaseCharacter::Update();
	Move();
	
	Relationship();

	/*----------落下処理----------*/
	
	if (!isHit_ && !isHitFloor_) {
		IsFall();
	}

	if (worldTransformBase_.translate.y < -150) {
		fallSpeed_ = 0.0f;
		worldTransformBase_.translate = {0.0f, 0.0f, 0.0f};
	}
	worldTransformBase_.UpdateMatrix();
	worldTransformHammer_.TransferMatrix();
	ImGui::Begin("Player");
	ImGui::DragFloat4("translation", &worldTransformBase_.translate.x, 0.01f);
	ImGui::Text(" X%f Y%f Z%f", worldTransformBase_.matWorld_.m[3][0], worldTransformBase_.matWorld_.m[3][1],worldTransformBase_.matWorld_.m[3][2]);
	ImGui::Text(" isHit%d", isHit_);
	ImGui::Text(" isHitFloor%d", isHitFloor_);
	ImGui::Text(" jump%d", jump_);
	ImGui::End();
	ImGui::Begin("Hammer");
	ImGui::DragFloat4("translation", &worldTransformHammer_.rotate.x, 0.01f);
	ImGui::End();
	ApplyGlobalVariables();
}

void Player::Draw(const ViewProjection& viewprojection){
	models_[0]->Draw(worldTransformBase_, viewprojection);
	if (behavior_ == Behavior::kAttack) {
		models_[1]->Draw(worldTransformHammer_, viewprojection);
	}
	
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
	worldPos.x = worldTransformBase_.matWorld_.m[3][0];
	worldPos.y = worldTransformBase_.matWorld_.m[3][1];
	worldPos.z = worldTransformBase_.matWorld_.m[3][2];
	return worldPos;
}

Vector3 Player::GetLocalPosition() {
	// ワールド座標を入れる関数
	Vector3 worldPos;
	// ワールド行列の平行移動成分を取得（ワールド座標）
	worldPos.x = worldTransformBase_.translate.x;
	worldPos.y = worldTransformBase_.translate.y;
	worldPos.z = worldTransformBase_.translate.z;
	return worldPos;
}

void Player::SetPosition(Vector3 Pos) { worldTransformBase_.translate = Pos; }

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
			    Math::MakeRotateXMatrix(viewProjection_->rotation_.x),
			    Math::Multiply(
			        Math::MakeRotateYMatrix(viewProjection_->rotation_.y),
			        Math::MakeRotateZMatrix(viewProjection_->rotation_.z)));
			// move = utility_->Normalize(move);
			move = Math::TransformNormal(move, rotateMatrix);

			if (isMove) {
				
				worldTransformBase_.translate =
				    Math::Add(worldTransformBase_.translate, move);
				destinationAngleY_ = std::atan2(move.x, move.z);
			}

			worldTransformBase_.rotate.y =
			    Math::LerpShortAngle(worldTransformBase_.rotate.y, destinationAngleY_, 0.2f);
	}

	//ジャンプ
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_A && !jump_) {
		    upSpeed_ = 0.6f;
		    jump_ = true;
	}
	Jump();
	// 攻撃
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_B) {
		    behaviorRequest_ = Behavior::kAttack;
	}

	// ダッシュボタンを押したら
	if (joyState.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		    behaviorRequest_ = Behavior::kDash;
	}
	
}

void Player::Move() {

	/*----------移動処理----------*/
	if (KeyInput::GetKey(DIK_D)) {
		worldTransformBase_.translate.x += 0.3f;

	}else if (KeyInput::GetKey(DIK_A)) {
		worldTransformBase_.translate.x -= 0.3f;
	}
	if (KeyInput::GetKey(DIK_W)) {
		worldTransformBase_.translate.z += 0.3f;

	} else if (KeyInput::GetKey(DIK_S)) {
		worldTransformBase_.translate.z -= 0.3f;
	}

	if (KeyInput::GetKey(DIK_UPARROW)) {
		worldTransformBase_.translate.y += 0.3f;

	} else if (KeyInput::GetKey(DIK_DOWNARROW)) {
		worldTransformBase_.translate.y -= 0.3f;
	}

	if (KeyInput::GetKey(DIK_SPACE) && !jump_) {
		upSpeed_ = 0.6f;
		jump_ = true;
	}
	Jump();

	if (KeyInput::GetKey(DIK_0)) {
		behaviorRequest_ = Behavior::kAttack;
	}
	
	if (KeyInput::GetKey(DIK_9)) {
		behaviorRequest_ = Behavior::kDash;
	}

}

void Player::Jump() {
	/*----------ジャンプ処理----------*/
	
	if (jump_) {
		worldTransformBase_.translate.y += upSpeed_;
	}

	if (isHit_ || isHitFloor_) {
		fallSpeed_ = 0.0f;
		upSpeed_ = 0.0f;
		jump_ = false;
		if (worldTransformBase_.translate.y < 0) {
			    worldTransformBase_.translate.y = 0;
		}
	}
}

// 攻撃初期化
void Player::AttackInitialize() {
	worldTransformHammer_.rotate.x = 0.0f;

	
	// アタックスピード
	attackSpeed = -0.1f;
	attack = false;
	attackTime = 1.0f;
	changeTime = 1.0f;
}
void Player::AttackUpdata() {
	worldTransformHammer_.rotate.x += attackSpeed;

	if (worldTransformHammer_.rotate.x <= -0.5f) {
		attack = true;
	}

	if (worldTransformHammer_.rotate.x > 1.5f) {
		attackSpeed = 0.0f;

		if (changeTime > 0) {
			changeTime -= 0.1f;
			

		} else {
			behaviorRequest_ = Behavior::kRoot;
		}

	} else if (attack) {
		attackTime -= 0.1f;
		attackSpeed = 0.0f;
		if (attackTime <= 0) {
			attackSpeed = 0.1f;
		}
	}
}

// ダッシュ初期化
void Player::DashInitialize() {
	workDash_.dashParameter_ = 0;
	worldTransformBase_.rotate.y = destinationAngleY_;
}
void Player::DashUpdata() {
	//dashTimer -= 4;
	
	Vector3 velocity = {0, 0, kDashSpeed};

	velocity = Math::TransformNormal(velocity, worldTransformBase_.matWorld_);
	worldTransformBase_.translate = Math::Add(worldTransformBase_.translate, velocity);

	

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void Player::Relationship(const WorldTransform& worldTransformFloor) {

	if (!worldTransformBase_.parent_) {

		// 移動床のワールド座標
		Vector3 objectWorldPos = {
		    worldTransformFloor.matWorld_.m[3][0], worldTransformFloor.matWorld_.m[3][1],
		    worldTransformFloor.matWorld_.m[3][2]};



		// プレイヤーローカル座標
		Matrix4x4 localPos =
		    Math::Multiply(Math::Inverse(worldTransformFloor.matWorld_), worldTransformBase_.matWorld_);

		Vector3 playerLocalPos = {localPos.m[3][0], localPos.m[3][1]+1.0f, localPos.m[3][2]};

		
		
		//ローカル座標系に入れる
		//worldTransformBase_.matWorld_ = localPos;
		worldTransformBase_.translate = playerLocalPos;

		Setparent(&worldTransformFloor);

		
	}
	
}

void Player::Setparent(const WorldTransform* parent) { 
	worldTransformBase_.parent_ = parent; 
	

};

void Player::DeleteParent() {
	if (worldTransformBase_.parent_) {
		worldTransformBase_.translate = worldTransformBase_.GetWorldPos();
		worldTransformBase_.parent_ = nullptr;
	}
}

void Player::IsFall() {

	fallSpeed_ -= 0.01f;
	upSpeed_ += fallSpeed_;
	worldTransformBase_.translate.y += upSpeed_;
}

void Player::OnCollision() {isHit_ = true;}

void Player::OutCollision() { isHit_ = false; }

void Player::OnCollisionFloor() { isHitFloor_ = true; }

void Player::OutCollisionFloor() { isHitFloor_ = false; }

void Player::Relationship() {
	worldTransformHammer_.matWorld_ =
	    Math::Multiply(Math::MakeAffineMatrix(
	        worldTransformHammer_.scale, worldTransformHammer_.rotate, worldTransformHammer_.translate),
	    worldTransformBase_.matWorld_);
}

void Player::ApplyGlobalVariables() {
#ifdef _DEBUG

	GlobalVariables* globalVaribles = GlobalVariables::GetInstance();
	const char* groupName = "Player";
	kDashSpeed = globalVaribles->GetFloatValue(groupName, "DashSpeed");
	
	
#endif
}