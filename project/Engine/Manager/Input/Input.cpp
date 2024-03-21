#include "Input.h"
#include "WinApp.h"
#include <cassert>

Input* Input::GetInstance() {
	static Input instance;
	return &instance;
}

void Input::Initialize() {
	HRESULT result;
	//	DirectInputの初期化
	directInput = nullptr;
	result = DirectInput8Create(
	    WinApp::GetWc().hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput,nullptr);
	assert(SUCCEEDED(result));
	
	//	キーボードの生成
	keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));
	//	入力データの形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard); // 標準形式
	assert(SUCCEEDED(result));
	//	排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
	    WinApp::GetHwnd(), DISCL_FOREGROUND | /*画面が手前にある場合のみ入力を受け付ける*/
	                           DISCL_NONEXCLUSIVE | /*デバイスをこのアプリだけで専有しない*/
	                           DISCL_NOWINKEY); /*Windowsキーを無効にする*/
	assert(SUCCEEDED(result));

	//	変数の初期化 指定した分だけ0にするZeroMemory
	ZeroMemory(&xInputState, sizeof(XINPUT_STATE));
	//	DWORDは32bitのuint型 XInputと型を合わせている
	DWORD dr = XInputGetState(0, &xInputState);
	//	接続があればのフラグ
	result == ERROR_SUCCESS ? isConnectPad = true : isConnectPad = false;
}

void Input::InputInitialize() { Input::GetInstance()->Initialize(); }

void Input::Update() {

	//キーボード情報の取得開始
	keyboard->Acquire();
	//全キーの入力状態を取得する
	//前フレームのキー状態の取得
	for (uint16_t i = 0; i < 256; i++) {
		oldKey[i] = key[i];
	}
	//今のキー状態の取得
	keyboard->GetDeviceState(sizeof(key),key);

	//キーの再取得
	oldXInputState = xInputState;
	DWORD dresult = XInputGetState(0, &xInputState);
	//接続状況の確認
	dresult == ERROR_SUCCESS ? isConnectPad = true : isConnectPad = false;
	if (isConnectPad) {

		// デッドzoneの設定
		if ((xInputState.Gamepad.sThumbLX < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		     xInputState.Gamepad.sThumbLX > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE) &&
		    (xInputState.Gamepad.sThumbLY < XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE &&
		     xInputState.Gamepad.sThumbLY > -XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE)) {
			xInputState.Gamepad.sThumbLX = 0;
			xInputState.Gamepad.sThumbLY = 0;
		}

		if ((xInputState.Gamepad.sThumbRX < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		     xInputState.Gamepad.sThumbRX > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE) &&
		    (xInputState.Gamepad.sThumbRY < XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE &&
		     xInputState.Gamepad.sThumbRY > -XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE)) {
			xInputState.Gamepad.sThumbRX = 0;
			xInputState.Gamepad.sThumbRY = 0;
		}
	}

}

bool Input::PressKey(uint8_t keynumber) { return Input::GetInstance()->key[keynumber]; }

bool Input::PushKey(uint8_t keynumber) {
	return Input::GetInstance()->key[keynumber] && !Input::GetInstance()->oldKey[keynumber];
}

bool Input::ReleaseKey(uint8_t keynumber) {
	return !Input::GetInstance()->key[keynumber] && Input::GetInstance()->oldKey[keynumber];
}

bool Input::GetPadConnect() { return isConnectPad; }

bool Input::GetPadButton(UINT button) { 
	if (xInputState.Gamepad.wButtons & button) {
		return true;
	}
	return false;
}

bool Input::GetPadButtonUp(UINT button) {
	return xInputState.Gamepad.wButtons != button && oldXInputState.Gamepad.wButtons == button;
}

bool Input::GetPadButtonDown(UINT button) {
	return xInputState.Gamepad.wButtons == button && oldXInputState.Gamepad.wButtons != button;
}

Vector2 Input::GetPadLStick() {
	SHORT x = xInputState.Gamepad.sThumbLX;
	SHORT y = xInputState.Gamepad.sThumbLY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

Vector2 Input::GetPadRStick() {
	SHORT x = xInputState.Gamepad.sThumbRX;
	SHORT y = xInputState.Gamepad.sThumbRY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}


bool Input::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const {
	DWORD result;
	result = XInputGetState(stickNo, &out);

	if (result == ERROR_SUCCESS) {
		SetJoyStickDeadZone(stickNo, out);
		return true;
	} else {
		return false;
	}
}

void Input::SetJoyStickDeadZone(int32_t stickNo, XINPUT_STATE& out) const {
	int LstickX = static_cast<int>(out.Gamepad.sThumbLX);
	int LstickY = static_cast<int>(out.Gamepad.sThumbLY);
	int RstickX = static_cast<int>(out.Gamepad.sThumbRX);
	int RstickY = static_cast<int>(out.Gamepad.sThumbRY);
	if (abs(LstickX) < DEADZONE) {
		LstickX = 0;
		out.Gamepad.sThumbLX = LstickX;
	}
	if (abs(LstickY) < DEADZONE) {
		LstickY = 0;
		out.Gamepad.sThumbLY = LstickY;
	}
	if (abs(RstickX) < DEADZONE) {
		RstickX = 0;
		out.Gamepad.sThumbRX = RstickX;
	}
	if (abs(RstickY) < DEADZONE) {
		RstickY = 0;
		out.Gamepad.sThumbRY = RstickY;
	}

}