#include "LockOn.h"

void LockOn::Initialize() {
	lockOnMark_.reset(Sprite::Create("resources/Reticle.png"));
	transform_.scale = {0.1f, 0.1f, 0.1f};
	transform_.translate = {(1280 / 2.0f), 720 / 2.0f, 0.0f};
}

void LockOn::Update(const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		// ロックオン状態
		if (target_) {
			// ロックオン解除
			if (KeyInput::GetPadConnect()) {
				if (KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_LEFT_THUMB) ||
				    KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER)
				   ) {
					target_ = nullptr;
				} else if (IsOutOfRange(enemies, viewProjection)) {
					target_ = nullptr;
				}
			}
			
			if (KeyInput::PushKey(DIK_L) || KeyInput::PushKey(DIK_J)) {
				target_ = nullptr;
			} else if (IsOutOfRange(enemies, viewProjection)) {
				target_ = nullptr;
			}

			// ターゲットを変える
			if (KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_X)) {
				searchTarget(enemies, viewProjection);
				targetIndex_++;
				if (targetIndex_ >= targets_.size()) {
					targetIndex_ = 0;
					target_ = nullptr;
					target_ = targets_[targetIndex_].second;
				} else {
					target_ = nullptr;
					target_ = targets_[targetIndex_].second;
				}
			}
			

		} else {
			if (KeyInput::GetPadConnect()) {
				// ロックオントリガー
				if (KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_LEFT_SHOULDER)) {
					searchTarget(enemies, viewProjection);
				}

				
			}

			// ロックオントリガー
			if (KeyInput::PushKey(DIK_L) ) {
				searchTarget(enemies, viewProjection);
			}
			// 自動ロックオン
			if (autoLockOn) {
				searchTarget(enemies, viewProjection);
			}

			
		}

		// ロックオン継続
		if (target_) {
			// 敵のロックオン座標
			Vector3 posWorld = target_->GetSenterPosition();
			// ワールド座標からスクリーン座標に変換
			Vector3 posScreen = WorldToScreen(viewProjection, posWorld);
			// Vector2に格納
			Vector2 posScreen2(posScreen.x -20, posScreen.y);
			// スプライトの座標を設定
			lockOnMark_->SetPosition(posScreen2);
			ImGui::Begin("Reticl");
			ImGui::DragFloat2("reticl", &posScreen2.x, 0.01f);
			ImGui::End();
		}
	}

	if (KeyInput::GetPadConnect()) {

		//自動ロックオン　オン・オフ
		if (KeyInput::GetPadButtonDown(XINPUT_GAMEPAD_LEFT_THUMB)) {
			if (autoLockOn) {
				autoLockOn = false;
			} else {
				autoLockOn = true;
			}
		}
	
	}

	if (KeyInput::PushKey(DIK_J)) {
		if (autoLockOn) {
			autoLockOn = false;
		} else {
			autoLockOn = true;
		}
	}
	

	ImGui::Begin("Reticl");
	ImGui::Text("autoLoackOn%d", autoLockOn);
	ImGui::End();
}

void LockOn::Draw() {
	Transform uv = {0.0f, 0.0f, 0.0f};
	if (target_) {
		lockOnMark_->Draw(transform_,uv );
	}
}

bool LockOn::IsOutOfRange(
    const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {

	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		// 敵のロックオン座標
		Vector3 posWorld;
		if (enemy->IsDead()) {
			posWorld = {0.0f, 0.0f, -1000.0f};
			
		} else {
			posWorld = enemy->GetSenterPosition();
		}

		// ワールドビュー座標変換
		Vector3 posView = Math::Transform(posWorld, viewProjection.matView);

		// 距離条件チェック
		if (minDistance_ <= posView.z && posView.z <= maxDistance_) {
			// カメラの前方との角度を計算
			//float arcTangent = std::atan2(std::sqrt(posView.x * posView.x + posView.y * posView.y), posView.z);
			float dot = Math::Dot({0.0f, 0.0f, 1.0f}, posView);
			float length = Math::Length(posView);
			float angle = std::acos(dot / length);
			// 角度条件チェック
			if (std::abs(angle) <= angleRenge_) {
				return false;
			}
		}
	}

	return true;
}

void LockOn::searchTarget(
    const std::list<std::unique_ptr<Enemy>>& enemies, const ViewProjection& viewProjection) {
	// 目標
	targets_.clear();

	// 全ての敵に対してロックオン判定
	for (const std::unique_ptr<Enemy>& enemy : enemies) {
		// 敵のロックオン座標
		Vector3 posWorld = enemy->GetSenterPosition();

		// ワールドビュー座標変換
		Vector3 posView = Math::Transform(posWorld, viewProjection.matView);

		// 距離条件チェック
		if (minDistance_ <= posView.z && posView.z <= maxDistance_) {
			// カメラの前方との角度を計算
		
			float dot = Math::Dot({0.0f, 0.0f, 1.0f}, posView);
			float length = Math::Length(posView);
			float angle = std::acos(dot / length);
			// 角度条件チェック
			if (std::abs(angle) <= angleRenge_) {
				targets_.emplace_back(std::make_pair(posView.z, enemy.get()));
			}
		}

		target_ = nullptr;
		if (targets_.size() != 0) {
			// 距離で昇順にソート
			std::sort(targets_.begin(), targets_.end(), [](auto& pair1, auto& pair2) {return pair1.first < pair2.first; });
			// ソートの結果1番地快適をロックオン対象とする
			target_ = targets_.front().second;
		}
	}
}

Vector3 LockOn::WorldToScreen(const ViewProjection& viewProjection, const Vector3& vector) {
	Vector3 vector_;

	// ビューポート
	Matrix4x4 matViewport =
	    Math::MakeViewportMatrix(0, 0, WinApp::kWindowWidth, WinApp::kWindowHeight, 0, 1);

	// ビューポート行列とプロジェクション行列、ビュー行列を合成
	Matrix4x4 matVPV = Math::Multiply(
	    Math::Multiply(viewProjection.matView, viewProjection.matProjection), matViewport);

	// ワールド座標からスクリーン座標変換（ここで3Dから2Dになる）
	vector_ = Math::Transform(vector, matVPV);

	return vector_;
}

Vector3 LockOn::GetTargetPos() const {
	if (target_) {
		return target_->GetSenterPosition();
	}

	return Vector3();
}