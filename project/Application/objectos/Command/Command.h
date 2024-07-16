#pragma once
#include <ViewProjection.h>
#include <Sprite.h>

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

class Command {
public:

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 毎フレーム処理
	/// </summary>
	void Update();

	void Draw(const ViewProjection& camera);

	void DrawUI();

	//こうげきフラグ
	bool GetAttack(int i) { return attackType_[i]; }

	//作戦フラグ
	bool GetOperatin() { return follow_; }

	//プレイヤーの状態
	void  SetBehavior(bool flag) { behaviorRoot_ = flag; }

private:
	//初期タスク
	void InitialInitialize();
	void InitialUpdate();
	void InitialTask();
	//作戦タスク
	void OperationInitialize();
	void OperationUpdate();
	void OperationTask();
	//呪文タスク
	void MagicInitialize();
	void MagicUpdate();
	void MagicTask();
	//特技タスク
	void SkillInitialize();
	void SkillUpdate();
	void SkillTask();
	//道具タスク
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
	int arrowMoveSpeed_ = 31;
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