#include "InputHandler.h"

void InputHandler::AssingMoveLeftCommand2PressKeyA() { 
	ICommand* command = new MoveLeftCommand(); 
	this->pressKeyA_ = command;
}

void InputHandler::AssingMoveRightCommand2PressKeyD() {
	ICommand* command = new MoveRightCommand();
	this->pressKeyD_ = command;
}

ICommand* InputHandler::HandleInput() {
	if (KeyInput::GetKey(DIK_D)) {
		return pressKeyD_;
	}

	if (KeyInput::GetKey(DIK_A)) {
		return pressKeyA_;
	}

	return nullptr;
}