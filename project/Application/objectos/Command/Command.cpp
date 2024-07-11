#include "Command.h"

void Command::Initialize() {
	backgroundTask.reset(Sprite::Create("resources/Command/task.png"));
	attack.reset(Sprite::Create("resources/Command/attack.png"));
	magic.reset(Sprite::Create("resources/Command/magic.png"));
	operation.reset(Sprite::Create("resources/Command/operation.png"));
	skill.reset(Sprite::Create("resources/Command/skill.png"));
	tool.reset(Sprite::Create("resources/Command/tool.png"));
	arrow.reset(Sprite::Create("resources/Command/arrow.png"));


	backgroundTask->SetColor({ 1.0f,1.0f,1.0f,0.8f});
	backgroundTask->SetPosition({30.0f,450.0f});

	attack->SetPosition({ 85.0f, 472.0f});
	attack->SetSize({ 91.0f,46.0f});

	magic->SetPosition({ 85.0f, 501.0f });
	magic->SetSize({ 91.0f,46.0f });

	operation->SetPosition({ 85.0f, 600.0f });
	operation->SetSize({ 91.0f,46.0f });

	skill->SetPosition({ 85.0f, 534.0f });
	skill->SetSize({ 91.0f,46.0f });

	tool->SetPosition({ 85.0f, 567.0f });
	tool->SetSize({ 91.0f,46.0f });

	arrowPos_ = { 31.0f,469.0f,0.0f };
	arrow->SetSize({ 50.0f,50.0 });
}

void Command::Update() {
	
	MoveArrow();
	

	ImGui::Begin("Command");
	ImGui::DragFloat2("Pos",&pos.x,1.0f);
	ImGui::DragFloat2("size", &size.x, 1.0f);
	ImGui::DragFloat3("ArrowPos", &arrowPos_.x, 1.0f);
	ImGui::End();
}

void Command::Draw(const ViewProjection& camera) {
	
}

void Command::DrawUI() {
	backgroundTask->Draw();
	attack->Draw();
	magic->Draw();
	operation->Draw();
	skill->Draw();
	tool->Draw();
	arrow->Draw();
}

void Command::MoveArrow() {
	arrow->SetPosition({arrowPos_.x, arrowPos_.y});

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