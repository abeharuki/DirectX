#pragma once
#include <ViewProjection.h>
#include <Sprite.h>

// コマンドクラスの定数
namespace CommandConstants {
	// 矢印の初期位置と移動範囲
	const float kArrowInitialX = 31.0f;//矢印UIの初期化X座標
	const float kArrowInitialY = 469.0f;//矢印UIの初期化Y座標
	const float kChangeTaskInitializeX = 31.0f;//矢印UIのタスクが変わった時の初期化X座標
	const float kChangeTaskInitializeY = 510.0f;//矢印UIのタスクが変わった時の初期化Y座標
	const float kArrowMoveSpeed = 31.0f;//矢印の動くスピード
	const float kArrowMaxY = 593.0f;//矢印の最大Y座標
	const float kArrowMinY = 469.0f;//矢印の最小Y座標
	const float kArrowAnimationStartX = 30.0f;//矢印アニメーションの折り返し地点最小
	const float kArrowAnimationEndX = 40.0f;//矢印アニメーションの折り返し地点最大
	const float kOperationArrowMaxY = 572.0f;//作戦タスク時の矢印の最大Y座標
	const float kOperationArrowMinY = 510.0f;//作戦タスク時の矢印の最小Y座標

	//各コマンドUIの初期化位置とサイズ
	const Vector2 kAttackPos = { 85.0f, 472.0f };//攻撃コマンドの座標
	const Vector2 kMagicPos = { 85.0f, 503.0f };//マジックコマンドの座標
	const Vector2 kSkillPos = { 85.0f, 534.0f };//スキルコマンドの座標
	const Vector2 kToolPos = { 85.0f, 565.0f };//アイテムコマンドの座標
	const Vector2 kOperationPos = { 85.0f, 596.0f };//作戦コマンドの座標
	const Vector2 kCommandSize = { 91.0f,46.0f };//コマンドのテクスチャのサイズ
	const Vector2 kChangeTaskCommand = { 85.0f, 472.0f };//タスクが切り替わった時のコマンド座標
	const Vector2 kTaskName = { 85.0f, 510.0f };//タスク名座標
	//作戦コマンドでの各コマンドの座標
	const Vector2 kEscapCommand = { 85.0f, 541.0f };//逃げるコマンドの座標
	const Vector2 kReturnCommand = { 85.0f, 572.0f };//前のタスクに戻るコマンドの座標


	// ボタンに対応する位置
	constexpr float kAttackButtonY = 468.0f;//攻撃コマンドの決定座標
	constexpr float kMagicButtonY = 499.0f;//マジックコマンドの決定座標
	constexpr float kSkillButtonY = 531.0f;//スキルコマンドの決定座標
	constexpr float kToolButtonY = 562.0f;//アイテムコマンドの決定座標
	constexpr float kOperationButtonY = 593.0f;//作戦コマンドの決定座標
	constexpr float kReturnToTaskButtonY = 510.0f;//元のタスクに戻るボタンの決定座標
	// 作戦タスクのボタンに対応する位置
	constexpr float kOperationAttackButtonY = 510.0f;//作戦コマンドの攻撃命令ボタン決定座標
	constexpr float kOperationFollowButtonY = 541.0f;//作成コマンドのついてくる命令ボタン決定座標
	constexpr float kOperationReturnToTaskButtonY = 572.0f;//元のタスクに戻るボタンの決定座標

	// 背景タスクのサイズと位置
	constexpr float kBackgroundTaskWidth = 200.0f;//タスクの横
	constexpr float kBackgroundTaskHeight = 200.0f;//タスクの高さ
	constexpr Vector2 kBackgroundTask1 = { 30.0f,450.0f };//タスク１の座標
	constexpr Vector2 kBackgroundTask1Size = { 310.0f,200.0f };//タスク1のサイズ
	constexpr Vector2 kBackgroundChangeTask1 = { 3.0f,450.0f };//タスク1のタスクが変わった時の座標
	constexpr Vector2 kBackgroundTask2 = {77.0f,474.0f};//タスク2の座標
	constexpr Vector2 kBackgroundTask2Size = { 167.0f,170.0f };//タスク２のサイズ
	constexpr Vector2 kBackgroundTask3 = { 37.0f,457.0f };//タスク3の座標
	constexpr Vector2 kBackgroundTask3Size = { 196.0f,200.0f };//タスク3のサイズ
	constexpr Vector2 kBackgroundAnimationTaskPos = { 30.0f,665.0f };//タスクのアニメーションpos
	constexpr float kBackgroundAnimationTaskSizeY = -199.9f;//タスクのアニメーションサイズ

}


enum priteType {
	kFight,
	kEscape,
	kReturn,
	kMax,
};

enum Task {
	kTask1,
	kTask2,
	kTask3,
	kTaskMax,
};

enum class TaskType {
	kInitial,
	kMagic,
	kSkill,
	kOperation,
	kTool,
};

enum AttackType {
	kNormalAttack,
	kAttackMax,
};

/**
 * @file Command
 * @brief プレーヤーが動かすコマンドUIを管理するクラス
 */
class Command {
public:

	// コマンドUI初期化
	void Initialize();
	// 毎フレーム処理
	void Update();
	//コマンドの描画(今は使われていない)
	//void Draw(const ViewProjection& camera);
	//コマンドUIの描画
	void DrawUI();

	//こうげきフラグ
	bool GetAttack(int i) { return attackType_[i]; }

	//作戦フラグ
	bool GetOperatin() { return follow_; }
	void SetOperatin(bool flag) { follow_ = flag; }

	//プレイヤーの状態
	void  SetBehavior(bool flag) { behaviorRoot_ = flag; }

	//状態の設定
	void SetTaskType(TaskType num) { behaviorRequest_ = num; taskAnimation_ = true; }
private:
	//初期タスクの初期化・更新・描画
	void InitialInitialize();
	void InitialUpdate();
	void InitialTask();
	//作戦タスク初期化・更新・描画
	void OperationInitialize();
	void OperationUpdate();
	void OperationTask();
	//呪文タスク初期化・更新・描画
	void MagicInitialize();
	void MagicUpdate();
	void MagicTask();
	//特技タスク初期化・更新・描画
	void SkillInitialize();
	void SkillUpdate();
	void SkillTask();
	//道具タスク初期化・更新・描画
	void ToolInitialize();
	void ToolUpdate();
	void ToolTask();

	//タスク背景のアニメーション
	void TaskAnimation();

	//矢印のアニメーションと動き
	void MoveArrow();
	void OperationMoveArrow();
	void MagicMoveArrow();
	void SkillMoveArrow();
	void ToolMoveArrow();

	void AnimationArrow();

private:
	std::vector<std::unique_ptr<Sprite>> backgroundTask;
	std::unique_ptr<Sprite> attack;
	std::unique_ptr<Sprite> magic;
	std::unique_ptr<Sprite> skill;
	std::unique_ptr<Sprite> tool;
	std::unique_ptr<Sprite> operation;
	std::unique_ptr<Sprite> arrow;

	std::vector<std::unique_ptr<Sprite>> sprites_;

	TaskType taskType_ = TaskType::kInitial;
	// 次の振る舞いリクエスト
	std::optional<TaskType> behaviorRequest_ = std::nullopt;

	Vector3 arrowPos_;
	bool arrowFlag_;

	std::vector<Vector2> pos;
	std::vector<Vector2> size;

	//タスクのアニメーション
	bool taskAnimation_ = false;
	Vector3 taskSize_ = { 0.0f,0.0f,0.0f };

	//playerの状態がmoveかどうか
	bool  behaviorRoot_ = true;

	//攻撃フラグ
	std::vector<bool> attackType_;

	//作戦フラグ
	bool follow_ = true;
};

