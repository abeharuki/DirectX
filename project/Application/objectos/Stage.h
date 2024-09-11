#pragma once
#include "Model.h"
#include "WorldTransform.h"
#include <list>
#include <CollisionManager/ColliderManager.h>
#include <Enemy/DebugEnemy.h>
#include <bitset>

class Stage {
public:
	// ステージの横方向の配列数
	static constexpr int32_t kSize_ = 16;
public:
	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	void Draw(ViewProjection viewprojection, bool light);



	void SetShake(Vector2 shake) {
		shake_ = shake;
	}

	void SetDebugEnemy(DebugEnemy *debugEnemy) { debugEnemy_ = debugEnemy; }
	Vector2 GetPlayerAttackPos(uint32_t i) { return Vector2(playerAttackTransform_[i].translate.x, playerAttackTransform_[i].translate.y); }
	Vector2 GetPlayerAttackScale(uint32_t i) { return Vector2(playerAttackTransform_[i].scale.x, playerAttackTransform_[i].scale.y); }
	Vector2 GetEnemyAttackPos(int i) { return Vector2(enemyAttackTransform_[i].translate.x, enemyAttackTransform_[i].translate.y); }
	Vector2 GetEnemyAttackScale(int i) { return Vector2(enemyAttackTransform_[i].scale.x, enemyAttackTransform_[i].scale.y); }
	void SetUp(bool up, int i) {
		if (!downBox_[i]) {
			upBox_[i] = true;
		}

	}
	void SetDown(bool up, int i) {
		if (!upBox_[i]) {
			downBox_[i] = true;
		}

	}
	bool GetUp(uint32_t index) const { return upBox_.at(index); }
	bool GetDown(uint32_t index) const { return downBox_.at(index); }

	/// @brief 指定している場所の棒が攻撃状態か検知する
	/// @param isUpside 上方向への攻撃かどうか
	/// @param index 指定するIndex
	/// @return 攻撃を行っているかどうか
	bool IsAttacking(const bool isUpside, uint32_t index) const {
		// 検知先の配列
		const std::array<bool, kSize_> &targetBox = isUpside ? upBox_ : downBox_;	// 上方向を検知するフラグの真偽で上の配列か下の配列かを切り替える

		// 指定したIndexが立っていて､なおかつ延長中である場合はtrueを返す
		return targetBox[index] and not returnScale_[index];
	}
	/// @brief 指定している場所の棒が攻撃状態か検知する
	/// @param isUpside 上方向への攻撃かどうか
	/// @return 攻撃を行っているかどうかの配列
	std::bitset<kSize_> IsAttacking(const bool isUpside) const {// 検知先の配列

		// 返却する配列データ
		std::bitset<kSize_> result;
		// 検知先の配列
		const std::array<bool, kSize_> &targetBox = isUpside ? upBox_ : downBox_;	// 上方向を検知するフラグの真偽で上の配列か下の配列かを切り替える

		for (uint32_t i = 0; i < kSize_; i++) {
			// 指定したIndexが立っていて､なおかつ延長中である場合はtrueを渡す
			result.set(i, targetBox[i] and not returnScale_[i]);
		}

		return result;
	}

private:

	Vector2 boxPos_[kSize_];
	Transform playerAttackTransform_[kSize_];
	Transform enemyAttackTransform_[kSize_];
	DebugEnemy *debugEnemy_;
	std::unique_ptr<Sprite> backGround_;
	std::unique_ptr<Sprite> boxSprite_[kSize_];
	std::unique_ptr<Sprite> playerAttackSprite_[kSize_];
	std::unique_ptr<Sprite> enemyAttackSprite_[kSize_];
	std::array<bool, kSize_> returnScale_;
	std::array<bool, kSize_> hitPlayer_;
	bool moveScale_;
	std::array<bool, kSize_> upBox_;
	std::array<bool, kSize_> downBox_;
	int boxNum_ = 0;
	Vector2 shake_;
	float posY[kSize_];
};