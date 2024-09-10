#include "DebugPlayer.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>
#include <GlobalVariables.h>
#include <Stage.h>

void DebugPlayer::Initialize() {

	playerStatus_.Load();
	playerStatus_.Save();

	// 初期化
	transform_.scale = { 0.5f,0.5f,0.5f };
	transform_.translate.x = 2.0f;
	transform_.translate.y = playerStatus_.stageeFloor_.second;

	sphere_ = std::make_unique<Sphere>();
	sphere_.reset(Sphere::CreateSphere("resources/white.png"));
	velocity_.x = 0.3f;

	animation_ = std::make_unique<Animations>();
	animation_.reset(Animations::Create("resources/human", "uvChecker.png", "Human.gltf"));
	animation_->SetAnimationTimer(0.0f, 30.0f);
	AABB aabbSize{ .min{-0.0f,-0.25f,-0.0f},.max{0.01f,0.25f,0.0f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);

	sprite_ = std::unique_ptr<Sprite>(Sprite::Create("resources/Player/player_animation.png"));

	sprite_->SetAnchorPoint(Vector2{ 0.5f,1.f });

	uvTransform_.scale = { 1.f / 9, 1.f / 6, 0.f };
}

void DebugPlayer::Update() {
#ifdef _DEBUG
	// プレイヤデータのロード
	playerStatus_.Load();

#endif // _DEBUG

	transform_.scale = Vector3{ 1.f, 40.f / 24.f ,1.f }*playerStatus_.drawScale_.second;
	animFlame_++;

	if (behaviorRequest_) {
		// 振る舞い変更
		behavior_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (behavior_) {
		case Behavior::kRoot:
		default:
			MoveInitialize();
			break;
		case Behavior::kJump:
			JumpInitialize();
			break;
		case Behavior::kAirJump:
			AirJumpInitialize();
			break;
		case Behavior::kHeadButt:
			HeadButtInitialize();
			break;
		case Behavior::kDash:
			DashInitialize();
			break;
		case Behavior::kAttack:
			AttackInitialize();
			break;

		}

		// 振る舞いリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (behavior_) {
	case Behavior::kRoot:
	default:
		// 通常行動
		MoveUpdate();
		break;
	case Behavior::kJump:
		//ジャンプ
		JumpUpdate();
		break;
	case Behavior::kAirJump:
		//ジャンプ
		AirJumpUpdate();
		break;
	case Behavior::kHeadButt:
		HeadButtUpdate();
		break;
	case Behavior::kDash:
		// ジャンプ
		DashUpdate();
		break;
	case Behavior::kAttack:
		// 攻撃
		AttackUpdate();
		break;

	}

	if (velocity_.x) { sprite_->SetFlipX(velocity_.x < 0.f); }

	// プレイヤのX軸移動範囲を制限
	if (transform_.translate.x <= -playerStatus_.stageWidth_.second) {	// 左方向
		transform_.translate.x = -playerStatus_.stageWidth_.second;
		if (velocity_.x < 0.f) { velocity_.x = 0.f; }
	}
	if (transform_.translate.x >= +playerStatus_.stageWidth_.second) {	// 右方向
		transform_.translate.x = playerStatus_.stageWidth_.second;
		if (velocity_.x > 0.f) { velocity_.x = 0.f; }
	}

	// プレイヤの地面へのめり込み対策
	if (transform_.translate.y <= playerStatus_.stageeFloor_.second) {
		transform_.translate.y = playerStatus_.stageeFloor_.second;
		if (velocity_.y < 0.f) { velocity_.y = 0.f; }
	}

	ImGui::Begin("Setting");
	ImGui::Text("posX%f", GetWorldPosition().x);
	ImGui::Text("posY%f", GetWorldPosition().y);
	ImGui::Text("posZ%f", GetWorldPosition().z);
	ImGui::DragFloat("PosY", &transform_.translate.y, 0.1f);
	ImGui::End();

	// トランスフォームから行列を計算する
	//CalcMatrix();
}

void DebugPlayer::Draw([[maybe_unused]] const ViewProjection &camera) {

	SetViewProjection(&camera);
	// スプライトにデータを転送する
	TransfarSprite();

	sprite_->UpdateVertexBuffer();
	// 画像の描画
	sprite_->Draw(uvTransform_);
}

// 移動
void DebugPlayer::MoveInitialize() {
	transform_.translate.y = playerStatus_.stageeFloor_.second;
	velocity_.y = 0.0f;
};
void DebugPlayer::MoveUpdate() {

	const float value = 0.7f;
	bool isMove_ = false;
	/*----------移動処理----------*/
	const float kCharacterSpeed = playerStatus_.groundMoveSpeed_.second;
	// 移動量
	velocity_ = { 0.0f, 0.0f, 0.0f };

	XINPUT_STATE joyState;

	if (Input::GetInstance()->GetJoystickState(0, joyState)) {

		constexpr float kDeadZone = 0.7f;
		bool isMove = false;

		// 移動量
		Vector3 velocity = {
		   std::abs(joyState.Gamepad.sThumbLX) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? (float)joyState.Gamepad.sThumbLX : 0.f / SHRT_MAX, 0.0f,
		   std::abs(joyState.Gamepad.sThumbLY) > XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE ? (float)joyState.Gamepad.sThumbLY : 0.f / SHRT_MAX };

		if (Math::Length(velocity) > kDeadZone) {
			velocity_ = Math::Normalize(velocity);
		}
	}

	if (Math::Length(velocity_) == 0.f) {
		// 左右移動
		if (Input::PressKey(DIK_A)) {
			velocity_.x--;
		}
		if (Input::PressKey(DIK_D)) {
			velocity_.x++;
		}
	}

	if (velocity_.x != 0) {
		isMove_ = true;
		velocity_ = Math::Normalize(velocity_);
		velocity_ *= kCharacterSpeed;
	}

	if (isMove_) {
		// 平行移動
		transform_.translate += velocity_;

		AnimUpdate(3, 3, 0, 2, true);
	}
	else {
		AnimUpdate(0, 4, 0, 6, true);
	}

	// ジャンプ
	if (Input::PushKey(DIK_SPACE)) {
		behaviorRequest_ = Behavior::kJump;
	}
};

// ジャンプ
void DebugPlayer::JumpInitialize() {

	// ジャンプ初速
	const float kJumpFirstSpeed = playerStatus_.jumpStrength_.second;
	velocity_.y = kJumpFirstSpeed;

	animFlame_ = 0;

};
void DebugPlayer::JumpUpdate() {

	if (velocity_.y > 0.f) {
		AnimUpdate(1, 6, 0, 2, false);
	}
	else {
		AnimUpdate(1, 6, 2, 3, true);
	}
	// 移動
	transform_.translate += velocity_;
	// 重力加速度
	const float kGravity = playerStatus_.gravity_.second;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;

	if (transform_.translate.y <= playerStatus_.stageeFloor_.second) {
		// 着地処理
		behaviorRequest_ = Behavior::kRoot;
	}

	else if (transform_.translate.y >= playerStatus_.stageHeight_.second) {
		// 衝突処理
		behaviorRequest_ = Behavior::kHeadButt;
	}
	else {
		Input *const pInput = Input::GetInstance();

		if (pInput->GetPadButtonDown(XINPUT_GAMEPAD_A) or pInput->PushKey(DIK_SPACE)) {
			behaviorRequest_ = Behavior::kAirJump;
		}


	}
}

void DebugPlayer::AirJumpInitialize()
{
	// ジャンプ初速
	const float kJumpFirstSpeed = playerStatus_.airJumpStrength_.second;
	velocity_.y = kJumpFirstSpeed;

	velocity_.x = 0.f;

	animFlame_ = 0;

}

void DebugPlayer::AirJumpUpdate()
{
	if (velocity_.y > 0.f) {
		AnimUpdate(1, 6, 0, 2, false);
	}
	else {
		AnimUpdate(1, 6, 2, 3, true);
	}

	// 移動
	transform_.translate += velocity_;
	// 重力加速度
	const float kGravity = playerStatus_.gravity_.second;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;

	if (transform_.translate.y >= playerStatus_.stageHeight_.second) {
		// 衝突処理
		behaviorRequest_ = Behavior::kHeadButt;
	}
}

void DebugPlayer::HeadButtInitialize()
{
	transform_.translate.y = playerStatus_.stageHeight_.second;
	velocity_.y = 0.f;

	// DxCommonからViewPortを取得する
	const DirectXCommon *const pDxCommon = DirectXCommon::GetInstance();
	const auto &vp = pDxCommon->GetViewPort();
	const int32_t sceneWidth = static_cast<int32_t>(vp.Width);

	// 画面の位置から座標を取得する
	const int32_t targetPos = std::clamp(static_cast<int32_t>(GetWorldPosition().x * Stage::kSize_ / (sceneWidth - 128 * 2)), 0, Stage::kSize_);

	// もしその場所が立ってなかったら
	if (not pStage_->GetUp(targetPos)) {
		pStage_->SetUp(true, targetPos);
	}
}

void DebugPlayer::HeadButtUpdate()
{
	// 移動
	transform_.translate += velocity_;
	// 重力加速度
	const float kGravity = playerStatus_.gravity_.second;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;
	if (transform_.translate.y <= playerStatus_.stageeFloor_.second) {
		// 着地処理
		behaviorRequest_ = Behavior::kRoot;
	}

}

// ダッシユ
void DebugPlayer::DashInitialize() {
	workDash_.dashParameter_ = 0;
	transform_.rotate.y = destinationAngleY_;
	dash_ = true;
}
void DebugPlayer::DashUpdate() {

	Vector3 velocity = { 0, 0, workDash_.dashSpeed };

	velocity = Math::TransformNormal(velocity, transMat_);
	transform_.translate += velocity;

	// 既定の時間経過で通常行動に戻る
	if (++workDash_.dashParameter_ >= workDash_.behaviorDashTime) {
		behaviorRequest_ = Behavior::kRoot;
	}
}


// 攻撃
void DebugPlayer::AttackInitialize() {}
void DebugPlayer::AttackUpdate() {
	animation_->Update(3);
}

void DebugPlayer::CalcMatrix()
{
	transMat_ = Math::MakeAffineMatrix(transform_.scale, transform_.rotate, transform_.translate);

}

void DebugPlayer::TransfarSprite()
{
	sprite_->SetPosition(pViewProjection_->WorldToScreen(transform_.translate).GetVec2() - Vector2{ .y = playerStatus_.drawOffset_.second });
	sprite_->SetRotation(transform_.rotate.z);
	sprite_->SetSize(transform_.scale.GetVec2());
}

void DebugPlayer::OnCollision(Collider *collider) {

	if (collider->GetCollisionAttribute() == kCollisionAttributeEnemy) {}

	if (collider->GetCollisionAttribute() == kCollisionAttributeLoderWall) {}

}

const Vector3 DebugPlayer::GetWorldPosition() const
{
	return { sprite_->GetPosition().x,  sprite_->GetPosition().y ,0.f };
}

DebugPlayer::~DebugPlayer() {}

bool DebugPlayer::AnimUpdate(const uint32_t layer, const uint32_t span, const uint32_t begin, const uint32_t end, bool isRoop)
{
	const uint32_t diff = end - begin;
	const uint32_t value = (isRoop ? (animFlame_ / span) % diff : std::min(animFlame_ / span, diff)) + begin;

	uvTransform_.translate.x = uvTransform_.scale.x * value;
	uvTransform_.translate.y = uvTransform_.scale.y * layer;

	return animFlame_ / span >= diff;

}

void PlayerStatus::Save() const
{
	// データの格納先
	GlobalVariables *gVal = GlobalVariables::GetInstance();
	gVal->AddItem(kGroupName_, groundMoveSpeed_.first, groundMoveSpeed_.second);
	gVal->AddItem(kGroupName_, jumpStrength_.first, jumpStrength_.second);
	gVal->AddItem(kGroupName_, airJumpStrength_.first, airJumpStrength_.second);
	gVal->AddItem(kGroupName_, gravity_.first, gravity_.second);
	gVal->AddItem(kGroupName_, stageHeight_.first, stageHeight_.second);
	gVal->AddItem(kGroupName_, stageWidth_.first, stageWidth_.second);
	gVal->AddItem(kGroupName_, stageeFloor_.first, stageeFloor_.second);
	gVal->AddItem(kGroupName_, drawScale_.first, drawScale_.second);
	gVal->AddItem(kGroupName_, drawOffset_.first, drawOffset_.second);
}

void PlayerStatus::Load()
{
	// データの格納先
	const GlobalVariables *gVal = GlobalVariables::GetInstance();

	gVal->GetValue(kGroupName_, groundMoveSpeed_.first, &groundMoveSpeed_.second);
	gVal->GetValue(kGroupName_, jumpStrength_.first, &jumpStrength_.second);
	gVal->GetValue(kGroupName_, airJumpStrength_.first, &airJumpStrength_.second);
	gVal->GetValue(kGroupName_, gravity_.first, &gravity_.second);
	gVal->GetValue(kGroupName_, stageHeight_.first, &stageHeight_.second);
	gVal->GetValue(kGroupName_, stageWidth_.first, &stageWidth_.second);
	gVal->GetValue(kGroupName_, stageeFloor_.first, &stageeFloor_.second);
	gVal->GetValue(kGroupName_, drawScale_.first, &drawScale_.second);
	gVal->GetValue(kGroupName_, drawOffset_.first, &drawOffset_.second);
}
