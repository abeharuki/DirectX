#include "Command.h"
#include <Input.h>
#include <GlobalVariables.h>

void Command::Initialize() {

	taskType_ = TaskType::kInitial;

	//初期タスク
	attack.reset(Sprite::Create("resources/Command/attack.png"));
	magic.reset(Sprite::Create("resources/Command/magic.png"));
	operation.reset(Sprite::Create("resources/Command/operation.png"));
	skill.reset(Sprite::Create("resources/Command/skill.png"));
	tool.reset(Sprite::Create("resources/Command/tool.png"));
	arrow.reset(Sprite::Create("resources/Command/arrow.png"));

	
	sprites_.resize(priteType::kMax);
	backgroundTask.resize(Task::kTaskMax);
	attackType_.resize(AttackType::kAttackMax);

	backgroundTask[kTask1].reset(Sprite::Create("resources/Command/task.png"));
	backgroundTask[kTask2].reset(Sprite::Create("resources/Command/task2.png"));
	backgroundTask[kTask3].reset(Sprite::Create("resources/Command/task3.png"));

	//作戦タスク
	sprites_[kFight].reset(Sprite::Create("resources/Command/fight.png"));
	sprites_[kEscape].reset(Sprite::Create("resources/Command/escape.png"));
	sprites_[kReturn].reset(Sprite::Create("resources/Command/return.png"));

	for (int i = 0; i < Task::kTaskMax; ++i) {
		backgroundTask[i]->SetColor(CommandConstants::kBackgroundTaskColor);
		backgroundTask[i]->SetPosition(CommandConstants::kBackgroundTask1);
	}
	
	for (int i = 0; i < priteType::kMax; ++i) {
		sprites_[i]->SetSize(CommandConstants::kCommandSize);
	}

	for (int i = 0; i < AttackType::kAttackMax; ++i) {
		attackType_[i] = false;
	}

	attack->SetPosition(CommandConstants::kAttackPos);
	attack->SetSize(CommandConstants::kCommandSize);

	magic->SetPosition(CommandConstants::kMagicPos);
	magic->SetSize(CommandConstants::kCommandSize);

	skill->SetPosition(CommandConstants::kSkillPos);
	skill->SetSize(CommandConstants::kCommandSize);

	tool->SetPosition(CommandConstants::kToolPos);
	tool->SetSize(CommandConstants::kCommandSize);

	operation->SetPosition(CommandConstants::kOperationPos);
	operation->SetSize(CommandConstants::kCommandSize);

	arrowPos_ = { CommandConstants::kArrowInitialX,CommandConstants::kArrowInitialY,0.0f };
	arrow->SetSize(CommandConstants::kArrowSize);

	backgroundTask[kTask1]->SetSize({ CommandConstants::kBackgroundTaskWidth,CommandConstants::kBackgroundTaskHeight} );

	pos.resize(3);
	size.resize(3);

}

void Command::Update() {
	
	if (behaviorRequest_) {
		// 振る舞い変更
		taskType_ = behaviorRequest_.value();
		// 各振る舞いごとの初期化
		switch (taskType_) {
		case TaskType::kInitial:
		default:
			InitialInitialize();
			break;
		case TaskType::kMagic:
			MagicInitialize();
			break;
		case TaskType::kOperation:
			OperationInitialize();
			break;
		case TaskType::kSkill:
			SkillInitialize();
			break;
		case TaskType::kTool:
			ToolInitialize();
			break;
		}

		// 振る舞いリセット
		behaviorRequest_ = std::nullopt;
	}

	switch (taskType_) {
	case TaskType::kInitial:
	default:
		InitialUpdate();
		break;
	case TaskType::kMagic:
		MagicUpdate();
		break;
	case TaskType::kOperation:
		OperationUpdate();
		break;
	case TaskType::kSkill:
		SkillUpdate();
		break;
	case TaskType::kTool:
		ToolUpdate();
		break;
	}

	AnimationArrow();
	

}

void Command::DrawUI() {
	switch (taskType_) {
	case TaskType::kInitial:
	default:
		InitialTask();
		break;
	case TaskType::kMagic:
		MagicTask();
		break;
	case TaskType::kOperation:
		OperationTask();
		break;
	case TaskType::kSkill:
		SkillTask();
		break;
	case TaskType::kTool:
		ToolTask();
		break;
	}
}

void Command::ApplyGlobalVariables()
{

}

void Command::InitialInitialize() {
	attack->SetPosition(CommandConstants::kAttackPos);
	magic->SetPosition(CommandConstants::kMagicPos);
	skill->SetPosition(CommandConstants::kSkillPos);
	tool->SetPosition(CommandConstants::kToolPos);
	operation->SetPosition(CommandConstants::kOperationPos);
	arrowPos_ = {CommandConstants::kArrowInitialX,CommandConstants::kArrowInitialY,0.0f};
	backgroundTask[kTask1]->SetPosition(CommandConstants::kBackgroundTask1);
	backgroundTask[kTask1]->SetSize({ CommandConstants::kBackgroundTaskWidth,CommandConstants::kBackgroundTaskHeight});
	taskSize_ = { CommandConstants::kBackgroundTaskWidth,0.0f,0.0f };
	for (int i = 0; i < AttackType::kAttackMax; ++i) {
		attackType_[i] = false;
	}
}

void Command::InitialUpdate(){
	if (!taskAnimation_) {
		MoveArrow();
	}
	TaskAnimation();
	

}

void Command::InitialTask(){
	backgroundTask[kTask1]->Draw();
	if (!taskAnimation_) {
		attack->Draw();
		magic->Draw();
		operation->Draw();
		skill->Draw();
		tool->Draw();
		arrow->Draw();
	}
}

void Command::OperationInitialize()
{
	operation->SetPosition( CommandConstants::kChangeTaskCommand);
	sprites_[kFight]->SetPosition(CommandConstants::kTaskName);
	sprites_[kEscape]->SetPosition(CommandConstants::kEscapCommand);
	sprites_[kReturn]->SetPosition(CommandConstants::kReturnCommand);

	backgroundTask[kTask3]->SetPosition(CommandConstants::kBackgroundTask3);
	backgroundTask[kTask3]->SetSize(CommandConstants::kBackgroundTask3Size);

	arrowPos_ = { CommandConstants::kChangeTaskInitializeX,CommandConstants::kChangeTaskInitializeY,0.0f };
	
}

void Command::OperationUpdate(){
	OperationMoveArrow();
}

void Command::OperationTask(){
	
	backgroundTask[kTask1]->Draw();
	backgroundTask[kTask3]->Draw();
	sprites_[kFight]->Draw();
	sprites_[kEscape]->Draw();
	sprites_[kReturn]->Draw();
	operation->Draw();
	arrow->Draw();
}

void Command::MagicInitialize(){
	magic->SetPosition(CommandConstants::kChangeTaskCommand);
	sprites_[kReturn]->SetPosition(CommandConstants::kTaskName);
	backgroundTask[kTask2]->SetPosition(CommandConstants::kBackgroundTask2);
	backgroundTask[kTask2]->SetSize(CommandConstants::kBackgroundTask2Size);
	backgroundTask[kTask1]->SetPosition(CommandConstants::kBackgroundChangeTask1);
	backgroundTask[kTask1]->SetSize(CommandConstants::kBackgroundTask1Size);
	arrowPos_ = { CommandConstants::kChangeTaskInitializeX,CommandConstants::kChangeTaskInitializeY,0.0f };
}

void Command::MagicUpdate(){
	MagicMoveArrow();
	
}

void Command::MagicTask(){
	backgroundTask[kTask1]->Draw();
	backgroundTask[kTask2]->Draw();
	magic->Draw();
	sprites_[kReturn]->Draw();
	arrow->Draw();
}

void Command::SkillInitialize(){
	skill->SetPosition(CommandConstants::kChangeTaskCommand);
	sprites_[kReturn]->SetPosition(CommandConstants::kTaskName);
	backgroundTask[kTask2]->SetPosition(CommandConstants::kBackgroundTask2);
	backgroundTask[kTask2]->SetSize(CommandConstants::kBackgroundTask2Size);
	backgroundTask[kTask1]->SetPosition(CommandConstants::kBackgroundChangeTask1);
	backgroundTask[kTask1]->SetSize(CommandConstants::kBackgroundTask1Size);
	arrowPos_ = { CommandConstants::kChangeTaskInitializeX,CommandConstants::kChangeTaskInitializeY,0.0f };
}

void Command::SkillUpdate(){
	SkillMoveArrow();
}

void Command::SkillTask(){
	backgroundTask[kTask1]->Draw();
	backgroundTask[kTask2]->Draw();
	skill->Draw();
	sprites_[kReturn]->Draw();
	arrow->Draw();
}

void Command::ToolInitialize(){
	tool->SetPosition(CommandConstants::kChangeTaskCommand);
	sprites_[kReturn]->SetPosition(CommandConstants::kTaskName);
	backgroundTask[kTask2]->SetPosition(CommandConstants::kBackgroundTask2);
	backgroundTask[kTask2]->SetSize(CommandConstants::kBackgroundTask2Size);
	backgroundTask[kTask1]->SetPosition(CommandConstants::kBackgroundChangeTask1);
	backgroundTask[kTask1]->SetSize(CommandConstants::kBackgroundTask1Size);
	arrowPos_ = { CommandConstants::kChangeTaskInitializeX,CommandConstants::kChangeTaskInitializeY,0.0f };
}

void Command::ToolUpdate(){
	ToolMoveArrow();
}

void Command::ToolTask(){
	backgroundTask[kTask1]->Draw();
	backgroundTask[kTask2]->Draw();
	tool->Draw();
	sprites_[kReturn]->Draw();
	arrow->Draw();
}

//タスクアニメーション
void Command::TaskAnimation(){
	if (taskAnimation_ && behaviorRoot_) {
		backgroundTask[kTask1]->SetSize({ taskSize_.x,taskSize_.y });
		backgroundTask[kTask1]->SetPosition(CommandConstants::kBackgroundAnimationTaskPos);
		taskSize_ = Math::Lerp({ Vector3{taskSize_.x,taskSize_.y,0.0f} }, Vector3{ taskSize_.x,CommandConstants::kBackgroundAnimationTaskSizeY,0.0f }, 0.4f);
		
		if (taskSize_.y <= CommandConstants::kBackgroundAnimationTaskSizeY + CommandConstants::kBackgroundAnimationOffsetY) {
			taskAnimation_ = false;
		}
	}
	

}

void Command::MoveArrow() {
	
	

	//矢印の移動
	if (Input::PushKey(DIK_1) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_UP)) {
		if (arrowPos_.y > CommandConstants::kArrowMinY) {
			arrowPos_.y = arrowPos_.y - CommandConstants::kArrowMoveSpeed;
		}
	
	}
	else if (Input::PushKey(DIK_2)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN)) {
		if (arrowPos_.y < CommandConstants::kArrowMaxY) {
			arrowPos_.y = arrowPos_.y + CommandConstants::kArrowMoveSpeed;
		}
	}

	//決定
	if (Input::PushKey(DIK_3)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
		if (int(arrowPos_.y) == CommandConstants::kAttackButtonY) {
			attackType_[kNormalAttack] = true;
			taskAnimation_ = true;
			taskSize_ = { CommandConstants::kBackgroundTaskWidth,0.0f};
			backgroundTask[kTask1]->SetSize({ taskSize_.x,taskSize_.y});
			behaviorRequest_ = TaskType::kInitial;
		}

		if (int(arrowPos_.y) == CommandConstants::kMagicButtonY) {
			behaviorRequest_ = TaskType::kMagic;
		}

		if (int(arrowPos_.y) == CommandConstants::kSkillButtonY) {
			behaviorRequest_ = TaskType::kSkill;
		}

		if (int(arrowPos_.y) == CommandConstants::kToolButtonY) {
			behaviorRequest_ = TaskType::kTool;
		}

		if (int(arrowPos_.y) == CommandConstants::kOperationButtonY) {
			behaviorRequest_ = TaskType::kOperation;
		}
	}


}

void Command::OperationMoveArrow(){
	//矢印の移動
	if (Input::PushKey(DIK_1)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_UP)) {
		if (arrowPos_.y > CommandConstants::kOperationArrowMinY) {
			arrowPos_.y = arrowPos_.y - CommandConstants::kArrowMoveSpeed;
		}

	}
	else if (Input::PushKey(DIK_2)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN)) {
		if (arrowPos_.y < CommandConstants::kOperationArrowMaxY) {
			arrowPos_.y = arrowPos_.y + CommandConstants::kArrowMoveSpeed;
		}
	}

	//決定
	if (Input::PushKey(DIK_3)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
		//こうげきの命令
		if (int(arrowPos_.y) == CommandConstants::kOperationAttackButtonY) {
			follow_ = false;
			taskAnimation_ = true;
			behaviorRequest_ = TaskType::kInitial;
		}
		//ついてくる命令
		if (int(arrowPos_.y) == CommandConstants::kOperationFollowButtonY) {
			follow_ = true;
			taskAnimation_ = true;
			behaviorRequest_ = TaskType::kInitial;
			
		}

		if (int(arrowPos_.y) == CommandConstants::kOperationReturnToTaskButtonY) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}

void Command::MagicMoveArrow(){
	//決定
	if (Input::PushKey(DIK_3) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
		
		if (int(arrowPos_.y) == CommandConstants::kReturnToTaskButtonY) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}

void Command::SkillMoveArrow(){
	//決定
	if (Input::PushKey(DIK_3) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {

		if (int(arrowPos_.y) == CommandConstants::kReturnToTaskButtonY) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}

void Command::ToolMoveArrow(){
	//決定
	if (Input::PushKey(DIK_3) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {

		if (int(arrowPos_.y) == CommandConstants::kReturnToTaskButtonY) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}


void Command::AnimationArrow(){
	arrow->SetPosition({ arrowPos_.x, arrowPos_.y });

	//矢印のアニメーション
	if (int(arrowPos_.x) == CommandConstants::kArrowAnimationStartX) {
		arrowFlag_ = true;
	}
	if (int(arrowPos_.x) == CommandConstants::kArrowAnimationEndX-CommandConstants::kArrowkAnimationOffset) {//39
		arrowFlag_ = false;
	}

	if (arrowFlag_) {
		arrowPos_ = Math::Lerp({ arrowPos_.x,arrowPos_.y,0.0f }, Vector3{ CommandConstants::kArrowAnimationEndX,arrowPos_.y,0.0f }, 0.1f);
	}
	else {
		arrowPos_ = Math::Lerp({ arrowPos_.x,arrowPos_.y,0.0f }, Vector3{ CommandConstants::kArrowAnimationStartX,arrowPos_.y,0.0f }, 0.1f);
	}
}
