#include "DebugPlayer.h"
#include <numbers>
#include <CollisionManager/CollisionConfig.h>
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

	AABB aabbSize{ .min{-0.0f,-0.25f,-0.0f},.max{0.01f,0.25f,0.0f} };
	SetAABB(aabbSize);
	SetCollisionPrimitive(kCollisionPrimitiveAABB);
	SetCollisionAttribute(kCollisionAttributePlayer);
	SetCollisionMask(kCollisionMaskPlayer);

	sprite_ = std::unique_ptr<Sprite>(Sprite::Create("resources/Player/player_animation.png"));

	sprite_->SetAnchorPoint(Vector2{ 0.5f,1.f });

	uvTransform_.scale = { 1.f / 9, 1.f / 6, 0.f };

	// 体力を最大にする
	health_ = playerStatus_.maxHealth_.second;
}

void DebugPlayer::Update() {
#ifdef _DEBUG
	// プレイヤデータのロード
	playerStatus_.Load();

#endif // _DEBUG

	transform_.scale = Vector3{ 1.f, 40.f / 24.f ,1.f }*playerStatus_.drawScale_.second;
	animFlame_++;

	// ステージの棒との当たり判定
	StageBarCollision();

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
		case Behavior::kKnockBack:
			KnockBackInitialize();
			break;
		case Behavior::kDead:
			DeadInitialize();
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
	case Behavior::kKnockBack:
		// 攻撃
		KnockBackUpdate();
		break;
	case Behavior::kDead:
		DeadUpdate();
		break;
	}

	if (velocity_.x) {
		// 左に動いているか
		const bool isMoveLeft = velocity_.x < 0.f;
		// ノックバック以外なら移動方向と同じ方向を向かせる
		sprite_->SetFlipX(behavior_ != Behavior::kKnockBack ? isMoveLeft : not isMoveLeft);
	}

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
	const auto &worldPos = GetWorldPosition();
	ImGui::Text("posX%f\nposY%f\nposZ%f", worldPos.x, worldPos.y, worldPos.z);
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

	// ステージ上においての座標を返す
	const int32_t targetPos = static_cast<int32_t>(GetOnStagePosX());

	// もしその場所が立ってなかったら
	if (not pStage_->GetUp(targetPos)) {
		pStage_->SetUp(true, targetPos);
		hitBlock_ = true;
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
	if (velocity_.y <= -0.3f) {
		hitBlock_ = false;
	}

	if (transform_.translate.y <= playerStatus_.stageeFloor_.second) {
		// 着地処理
		behaviorRequest_ = Behavior::kRoot;
		//hitBlock_ = false;
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
}

void DebugPlayer::KnockBackInitialize()
{
	animFlame_ = 0;
}

void DebugPlayer::KnockBackUpdate()
{
	// ノックバック時間の間隔
	const uint32_t animSpan = 5;
	AnimUpdate(4, animSpan, 0, 2, false);

	// もし床に触れていたら横へのベクトルを減速させる
	if (transform_.translate.y <= playerStatus_.stageeFloor_.second) { velocity_.x *= 0.75f; }
	// もし天井に触れていたら落とす
	if (transform_.translate.y > playerStatus_.stageHeight_.second) {
		transform_.translate.y = playerStatus_.stageHeight_.second;
		velocity_.y = 0.f;
	}

	// 移動
	transform_.translate += velocity_;
	// 重力加速度
	const float kGravity = playerStatus_.gravity_.second;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;

	// ノックバック時間が終了したら
	if (animFlame_ >= static_cast<uint32_t>(playerStatus_.knockBackFlame_.second)) {
		behaviorRequest_ = Behavior::kRoot;
	}
}

void DebugPlayer::DeadInitialize()
{
	animFlame_ = 0;
}

void DebugPlayer::DeadUpdate()
{
	AnimUpdate(5, 3, 0, 8, false);
	// 移動
	transform_.translate.y += velocity_.y;
	// 重力加速度
	const float kGravity = playerStatus_.gravity_.second;
	// 加速ベクトル
	Vector3 accelerationVector = { 0, -kGravity, 0 };
	// 加速
	velocity_ += accelerationVector;

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

void DebugPlayer::StageBarCollision()
{
	// ノックバックと死亡状態以外であれば
	if (behavior_ != Behavior::kKnockBack and behavior_ != Behavior::kDead) {

		// 攻撃状態の棒のリスト
		const std::bitset<Stage::kSize_> isBarAttack = pStage_->IsAttacking(false);
		// プレイヤの座標
		const float playerPos = GetOnStagePosX() - 0.5f;
		// プレイヤの両端のIndex
		const std::array<uint32_t, 2> playerSidePos{
			static_cast<uint32_t>(std::floor(playerPos)),
			static_cast<uint32_t>(std::ceil(playerPos))
		};
		for (uint32_t target : playerSidePos) {
			// どちらかが攻撃範囲に入ってたら
			if (target >= 0 and target < Stage::kSize_ and isBarAttack[target]) {
				// ダメージ判定して終わる
				OnCollision(target);
				break;
			}
		}
	}
}

void DebugPlayer::OnCollision(const uint32_t index) {

	// プレイヤの座標を取得する
	const float playerPos = GetOnStagePosX();

	// 棒の座標からどれくらいの距離にいるのかを返す
	const float posDiff = (playerPos - 0.5f) - index;

	// ベクトルを与える
	velocity_ = playerStatus_.knockBackPower.second;

	// もし左側なら
	if (posDiff < 0.f) {
		// ベクトルを反転させる
		velocity_.x *= -1.f;
	}

	// プレイヤの体力を削る
	health_--;

	// もし体力がまだ残っていたら
	if (health_ > 0) {
		// プレイヤの状態をノックバックにする
		behaviorRequest_ = Behavior::kKnockBack;
	}
	else {
		// プレイヤの状態を死亡状態にする
		behaviorRequest_ = Behavior::kDead;
	}

}

void DebugPlayer::OnCollision([[maybe_unused]] Collider *collider) {

}

const Vector3 DebugPlayer::GetWorldPosition() const
{
	return { sprite_->GetPosition().x,  sprite_->GetPosition().y ,0.f };
}

float DebugPlayer::GetOnStagePosX() const
{
	// DxCommonからViewPortを取得する
	const DirectXCommon *const pDxCommon = DirectXCommon::GetInstance();
	const auto &vp = pDxCommon->GetViewPort();
	const int32_t sceneWidth = static_cast<int32_t>(vp.Width);

	// 画面の位置から座標を取得する
	return std::clamp(GetWorldPosition().x * Stage::kSize_ / (sceneWidth - 128 * 2), 0.f, static_cast<float>(Stage::kSize_));
}

bool DebugPlayer::IsGameOver() const
{
	return
		// 体力が0になってるか
		health_ <= 0 and
		// 死亡状態である
		behavior_ == Behavior::kDead and
		// 死亡の待機時間が終わっている
		animFlame_ >= static_cast<uint32_t>(playerStatus_.deadFlame.second);
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
	LoadHelper helper{ GlobalVariables::GetInstance() };
	helper << groundMoveSpeed_;
	helper << jumpStrength_;
	helper << airJumpStrength_;
	helper << gravity_;
	helper << stageHeight_;
	helper << stageWidth_;
	helper << stageeFloor_;
	helper << drawScale_;
	helper << drawOffset_;
	helper << knockBackFlame_;
	helper << knockBackPower;
	helper << maxHealth_;
}

void PlayerStatus::Load()
{
	// データの格納先
	const LoadHelper helper{ GlobalVariables::GetInstance() };
	helper >> groundMoveSpeed_;
	helper >> jumpStrength_;
	helper >> airJumpStrength_;
	helper >> gravity_;
	helper >> stageHeight_;
	helper >> stageWidth_;
	helper >> stageeFloor_;
	helper >> drawScale_;
	helper >> drawOffset_;
	helper >> knockBackFlame_;
	helper >> knockBackPower;
	helper >> maxHealth_;
}
