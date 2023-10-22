#include "KeyInput.h"
#include "WinApp.h"
#include <cassert>

KeyInput* KeyInput::GetInstance() {
	static KeyInput instance;
	return &instance;
}

void KeyInput::Initialize() {
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

void KeyInput::InputInitialize() { KeyInput::GetInstance()->Initialize(); }

void KeyInput::Update() {

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

bool KeyInput::GetKey(uint8_t keynumber) { return KeyInput::GetInstance()->key[keynumber]; }

bool KeyInput::PushKey(uint8_t keynumber) {
	return KeyInput::GetInstance()->key[keynumber] && !KeyInput::GetInstance()->oldKey[keynumber];
}

bool KeyInput::ReleaseKey(uint8_t keynumber) {
	return !KeyInput::GetInstance()->key[keynumber] && KeyInput::GetInstance()->oldKey[keynumber];
}

bool KeyInput::GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const {
	DWORD result;
	result = XInputGetState(stickNo, &out);

	if (result == ERROR_SUCCESS) {
		SetJoyStickDeadZone(stickNo, out);
		return true;
	} else {
		return false;
	}
}

void KeyInput::SetJoyStickDeadZone(int32_t stickNo, XINPUT_STATE& out) const {
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


bool KeyInput::GetPadConnect() { return isConnectPad; }

bool KeyInput::GetPadButton(UINT button) { return xInputState.Gamepad.wButtons == button; }

bool KeyInput::GetPadButtonUp(UINT button) {
	return xInputState.Gamepad.wButtons != button && oldXInputState.Gamepad.wButtons == button;
}

bool KeyInput::GetPadButtonDown(UINT button) {
	return xInputState.Gamepad.wButtons == button && oldXInputState.Gamepad.wButtons != button;
}

Vector2 KeyInput::GetPadLStick() {
	SHORT x = xInputState.Gamepad.sThumbLX;
	SHORT y = xInputState.Gamepad.sThumbLY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

Vector2 KeyInput::GetPadRStick() {
	SHORT x = xInputState.Gamepad.sThumbRX;
	SHORT y = xInputState.Gamepad.sThumbRY;

	return Vector2(static_cast<float>(x) / 32767.0f, static_cast<float>(y) / 32767.0f);
}

bool KeyInput::GetLTriggerDown() {
	//	デッドラインの設定必須
	if (oldXInputState.Gamepad.bLeftTrigger < 128 && xInputState.Gamepad.bLeftTrigger >= 128) {
		return true;
	}
	return false;
}

bool KeyInput::GetRTriggerDown() {
	//	デッドラインの設定必須
	if (oldXInputState.Gamepad.bRightTrigger < 128 && xInputState.Gamepad.bRightTrigger >= 128) {
		return true;
	}
	return false;
}
bool KeyInput::GetLTrigger() {
	//	デッドラインの設定必須
	if (xInputState.Gamepad.bLeftTrigger >= 128) {
		return true;
	}
	return false;
}

bool KeyInput::GetRTrigger() {
	//	デッドラインの設定必須
	if (xInputState.Gamepad.bRightTrigger >= 128) {
		return true;
	}
	return false;
}