#pragma once
#include "DirectXCommon.h"
#include "Engine.h"
#include "Input.h"
#include "Sprite.h"
#include "ViewProjection.h"
#include "WorldTransform.h"
#include <array>
#include <optional>
#include "CollisionManager/Collider.h"
#include <Animation/Animation.h>
#include "PostEffects/PostEffect.h"
#include <Sphere.h>
#include "Player/DebugPlayer.h"

class DebugEnemy {
public: // メンバ関数

	/// <summary>
	/// デストラクタ
	/// </summary>
	~DebugEnemy();


	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection &camera);

	// 移動
	void MoveInitialize();
	void MoveUpdata();

	// ジャンプ
	void JumpInitialize();
	void JumpUpdata();

	//ダッシュ
	void DashInitialize();
	void DashUpdata();

	//攻撃
	void AttackInitialize();
	void AttackUpdata();

	//死亡
	void DeadInitilize();
	void DeadUpdata();

	//攻撃を食らったときの計算
	void Damage();

	/*ゲッターとセッター*/
	void SetViewProjection(const ViewProjection *viewProjection) { viewProjection_ = viewProjection; }
	void SetDebugPlayer(DebugPlayer *debugPlayer) { debugPlayer_ = debugPlayer; }
	void SetHitBody(bool hit) { hitBody_ = hit; }
	void SetHitCore(bool hit) { hitCore_ = hit; }
	void SetShake(bool flag) { shake_ = flag; }
	void SetBarr(Vector2 pos, Vector2 scale, uint32_t i) { transformBarr_[i].translate = Vector3{ pos.x,pos.y,0.0f };  transformBarr_[i].scale = Vector3{ scale.x,scale.y,0.0f }; }
	bool GetAttack() { return attack_; }
	Vector2 GetPos() { return Vector2{ transformBase_.translate.x,transformBase_.translate.y }; }
	Vector2 GetScale() { return Vector2{ transformBase_.scale.x,transformBase_.scale.y }; }
	Vector2 GetCorePos() { return Vector2{ transformCore_.translate.x,transformCore_.translate.y }; }
	Vector2 GetCoreScale() { return Vector2{ transformCore_.scale.x,transformCore_.scale.y }; }
	float GetHpLength() const { return (transformUI_[1].scale.x / 494.0f); }

	//押し出し関数
	void ResolveBoxCollision(float AposX, float AposY, const float AsizeW, const float AsizeH,
		float &BposX, float &BposY, const float BsizeW, const float BsizeH)
	{
		// 衝突判定を行う
		if (Math::IsBoxCollision(AposX, AposY, AsizeW, AsizeH, BposX, BposY, BsizeW, BsizeH)) {
			// 重なり量の計算
			float overlapX1 = (AposX + AsizeW) - BposX; // Aが左側からBに重なっている場合の重なり量
			float overlapX2 = (BposX + BsizeW) - AposX; // Aが右側からBに重なっている場合の重なり量
			float overlapY1 = (AposY + AsizeH) - BposY; // Aが上側からBに重なっている場合の重なり量
			float overlapY2 = (BposY + BsizeH) - AposY; // Aが下側からBに重なっている場合の重なり量
			hitBody_ = true;

			if (hitBody_ != preHitBody_) {
				// AがBの下から当たっている場合は押し出し処理を行わない
				if (overlapY2 < overlapY1) {
					// 横方向の押し出し処理のみを行う
					if (overlapX1 < overlapX2) {
						// Aを左側に押し出す
						transformBase_.translate.x -= overlapX1;
						if (velocity_.x > 0) {
							velocity_.x = 0; // 右向きの速度をリセット
						}
					}
					else {
						// Aを右側に押し出す
						transformBase_.translate.x += overlapX2;
						if (velocity_.x < 0) {
							velocity_.x = 0; // 左向きの速度をリセット
						}
					}
				}


			}


		}

	}


private: // メンバ変数
	Transform transformBase_;
	Transform transformCore_;
	std::array<Transform, 3> transformUI_;
	std::array<Transform, 16> transformBarr_;
	Vector2 coreSub_;
	const ViewProjection *viewProjection_;
	std::unique_ptr<Sprite> enemySprite_;
	std::unique_ptr<Sprite> enemyUI_[3];

	DebugPlayer *debugPlayer_;


	// 振る舞い
	enum class Behavior {
		kRoot, // 通常状態
		kJump, // ジャンプ
		kDash, // ダッシュ
		kAttack, //攻撃
		knock,//ノックバック
		kDead, // 死亡
	};

	Behavior behavior_ = Behavior::kRoot;
	// 次の振る舞いリクエスト
	std::optional<Behavior> behaviorRequest_ = std::nullopt;

	bool attack_;
	bool jump_;
	bool jumpAttack_;
	bool hitBody_;
	bool preHitBody_;
	bool hitCore_;
	bool preHitCore_;
	bool shake_;

	// ジャンプの時間
	uint32_t behaviorJumpTime = 30;
	//攻撃までの時間(クールタイム)
	uint32_t behaviorAttackTime = 30;
	//移動可能になるまでの時間
	uint32_t behaviorMoveTime = 20;
	int behaviorStopTime = 30;
	// 速度
	Vector3 velocity_ = {};

	AudioHelper damageSound_;
};