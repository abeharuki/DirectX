#include "Command.h"
#include <Input.h>

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
		backgroundTask[i]->SetColor({ 1.0f,1.0f,1.0f,0.8f });
		backgroundTask[i]->SetPosition({ 30.0f,450.0f });
	}
	

	for (int i = 0; i < priteType::kMax; ++i) {
		sprites_[i]->SetSize({ 91.0f,46.0f });
	}

	for (int i = 0; i < AttackType::kAttackMax; ++i) {
		attackType_[i] = false;
	}

	attack->SetPosition({ 85.0f, 472.0f});
	attack->SetSize({ 91.0f,46.0f});

	magic->SetPosition({ 85.0f, 503.0f });
	magic->SetSize({ 91.0f,46.0f });

	skill->SetPosition({ 85.0f, 534.0f });
	skill->SetSize({ 91.0f,46.0f });

	tool->SetPosition({ 85.0f, 565.0f });
	tool->SetSize({ 91.0f,46.0f });

	operation->SetPosition({ 85.0f, 596.0f });
	operation->SetSize({ 91.0f,46.0f });

	arrowPos_ = { 31.0f,469.0f,0.0f };
	arrow->SetSize({ 50.0f,50.0 });

	backgroundTask[kTask1]->SetSize({200.0f,200.0f} );

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
	
	

	ImGui::Begin("Command");
	ImGui::DragFloat2("Pos1", &pos[0].x, 1.0f);
	ImGui::DragFloat2("size1", &size[0].x, 1.0f);
	ImGui::DragFloat2("Pos2", &pos[1].x, 1.0f);
	ImGui::DragFloat2("size2", &size[1].x, 1.0f);
	ImGui::DragFloat2("Pos3", &pos[2].x, 1.0f);
	ImGui::DragFloat2("size3", &size[2].x, 1.0f);
	ImGui::DragFloat2("Pos3", &pos[2].x, 1.0f);
	ImGui::DragFloat2("size3", &size[2].x, 1.0f);
	ImGui::DragFloat3("ArrowPos", &arrowPos_.x, 1.0f);
	ImGui::Text("TaskSize%f", taskSize_.y);
	ImGui::Text("TaskAnimation%d", taskAnimation_);
	ImGui::End();
}

void Command::Draw(const ViewProjection& camera) {
	camera;
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

void Command::InitialInitialize(){
	attack->SetPosition({ 85.0f, 472.0f });
	magic->SetPosition({ 85.0f, 503.0f });
	skill->SetPosition({ 85.0f, 534.0f });
	tool->SetPosition({ 85.0f, 565.0f });
	operation->SetPosition({ 85.0f, 596.0f });
	arrowPos_ = { 31.0f,469.0f,0.0f };
	backgroundTask[kTask1]->SetPosition({ 30.0f,450.0f });
	backgroundTask[kTask1]->SetSize({200.0f,200.0f});
	taskSize_ = { 200.0f,0.0f,0.0f };
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
	operation->SetPosition({ 85.0f, 472.0f });
	sprites_[kFight]->SetPosition({85.0f,510.0f});
	sprites_[kEscape]->SetPosition({ 85.0f,541.0f });
	sprites_[kReturn]->SetPosition({ 85.0f,572.0f });

	backgroundTask[kTask3]->SetPosition({37.0f,457.0f});
	backgroundTask[kTask3]->SetSize({196.0f,200.0f});

	arrowPos_ = { 31.0f,510.0f,0.0f };
	
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
	magic->SetPosition({ 85.0f, 472.0f });
	sprites_[kReturn]->SetPosition({ 85.0f,510.0f });
	backgroundTask[kTask2]->SetPosition({77.0f,474.0f});
	backgroundTask[kTask2]->SetSize({167.0f,170.0f});
	backgroundTask[kTask1]->SetPosition({ 3.0f,450.0f });
	backgroundTask[kTask1]->SetSize({ 310.0f,200.0f });
	arrowPos_ = { 31.0f,510.0f,0.0f };
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
	skill->SetPosition({ 85.0f, 472.0f });
	sprites_[kReturn]->SetPosition({ 85.0f,510.0f });
	backgroundTask[kTask2]->SetPosition({ 77.0f,474.0f });
	backgroundTask[kTask2]->SetSize({ 167.0f,170.0f });
	backgroundTask[kTask1]->SetPosition({ 3.0f,450.0f });
	backgroundTask[kTask1]->SetSize({ 310.0f,200.0f });
	arrowPos_ = { 31.0f,510.0f,0.0f };
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
	tool->SetPosition({ 85.0f, 472.0f });
	sprites_[kReturn]->SetPosition({ 85.0f,510.0f });
	backgroundTask[kTask2]->SetPosition({ 77.0f,474.0f });
	backgroundTask[kTask2]->SetSize({ 167.0f,170.0f });
	backgroundTask[kTask1]->SetPosition({ 3.0f,450.0f });
	backgroundTask[kTask1]->SetSize({ 310.0f,200.0f });
	arrowPos_ = { 31.0f,510.0f,0.0f };
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
		backgroundTask[kTask1]->SetPosition({ 30.0f,665.0f });
		taskSize_ = Math::Lerp({ Vector3{taskSize_.x,taskSize_.y,0.0f} }, Vector3{ taskSize_.x,-200.0f,0.0f }, 0.4f);
		
		if (taskSize_.y <= -199.9f) {
			taskAnimation_ = false;
		}
	}
	

}

void Command::MoveArrow() {
	
	

	//矢印の移動
	if (Input::PushKey(DIK_1) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_UP)) {
		if (arrowPos_.y > 469.0f) {
			arrowPos_.y = arrowPos_.y - arrowMoveSpeed_;
		}
	
	}
	else if (Input::PushKey(DIK_2)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN)) {
		if (arrowPos_.y < 593.0f) {
			arrowPos_.y = arrowPos_.y + arrowMoveSpeed_;
		}
	}

	//決定
	if (Input::PushKey(DIK_3)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
		if (int(arrowPos_.y) == 468) {
			attackType_[kNormalAttack] = true;
			taskAnimation_ = true;
			taskSize_ = { 200.0f,0.0f,0.0f };
			backgroundTask[kTask1]->SetSize({ taskSize_.x,taskSize_.y});
			behaviorRequest_ = TaskType::kInitial;
		}

		if (int(arrowPos_.y) == 499) {
			behaviorRequest_ = TaskType::kMagic;
		}

		if (int(arrowPos_.y) == 531) {
			behaviorRequest_ = TaskType::kSkill;
		}

		if (int(arrowPos_.y) == 562) {
			behaviorRequest_ = TaskType::kTool;
		}

		if (int(arrowPos_.y) == 593) {
			behaviorRequest_ = TaskType::kOperation;
		}
	}


}

void Command::OperationMoveArrow(){
	//矢印の移動
	if (Input::PushKey(DIK_1)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_UP)) {
		if (arrowPos_.y > 510.0f) {
			arrowPos_.y = arrowPos_.y - arrowMoveSpeed_;
		}

	}
	else if (Input::PushKey(DIK_2)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_DPAD_DOWN)) {
		if (arrowPos_.y < 572.0f) {
			arrowPos_.y = arrowPos_.y + arrowMoveSpeed_;
		}
	}

	//決定
	if (Input::PushKey(DIK_3)|| Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
		//こうげきの命令
		if (int(arrowPos_.y) == 510) {
			follow_ = false;
			taskAnimation_ = true;
			behaviorRequest_ = TaskType::kInitial;
		}
		//ついてくる命令
		if (int(arrowPos_.y) == 541) {
			follow_ = true;
			taskAnimation_ = true;
			behaviorRequest_ = TaskType::kInitial;
			
		}

		if (int(arrowPos_.y) == 572) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}

void Command::MagicMoveArrow(){
	//決定
	if (Input::PushKey(DIK_3) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {
		
		if (int(arrowPos_.y) == 510) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}

void Command::SkillMoveArrow(){
	//決定
	if (Input::PushKey(DIK_3) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {

		if (int(arrowPos_.y) == 510) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}

void Command::ToolMoveArrow(){
	//決定
	if (Input::PushKey(DIK_3) || Input::GetInstance()->GetPadButtonDown(XINPUT_GAMEPAD_B)) {

		if (int(arrowPos_.y) == 510) {
			behaviorRequest_ = TaskType::kInitial;
		}

	}
}


void Command::AnimationArrow(){
	arrow->SetPosition({ arrowPos_.x, arrowPos_.y });

	//矢印のアニメーション
	if (int(arrowPos_.x) == 30) {
		arrowFlag_ = true;
	}
	if (int(arrowPos_.x) == 39) {
		arrowFlag_ = false;
	}

	if (arrowFlag_) {
		arrowPos_ = Math::Lerp({ arrowPos_.x,arrowPos_.y,0.0f }, Vector3{ 40.0f,arrowPos_.y,0.0f }, 0.1f);
	}
	else {
		arrowPos_ = Math::Lerp({ arrowPos_.x,arrowPos_.y,0.0f }, Vector3{ 30.0f,arrowPos_.y,0.0f }, 0.1f);
	}
}
