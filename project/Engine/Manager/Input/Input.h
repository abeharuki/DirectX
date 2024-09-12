#pragma once

#define DIRECTINPUT_VERSION 0x0800 // DirectInputのバージョン指定
#include <dinput.h>

#include <Xinput.h>
#include <cstdint>
#include <wrl.h>
#include "Math/math.h"
using namespace Microsoft::WRL;

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "xinput.lib")

class Input {
public:
	Input() = default;
	~Input() = default;

	
	static Input* GetInstance();

private:
	

public:
	//	通常初期化処理
	void Initialize();

	//	静的初期化処理
	static void InputInitialize();

	// 更新処理
	void Update();

private: // 変数
	//	DirectInputの初期化
	Microsoft::WRL::ComPtr<IDirectInput8> directInput;
	//	キーボードの生成
	Microsoft::WRL::ComPtr<IDirectInputDevice8> keyboard;
	//	キーの用意
	BYTE key[256] = {};
	BYTE oldKey[256] = {};
	//	コントローラーの用意
	XINPUT_STATE xInputState = {};
	XINPUT_STATE oldXInputState = {};
	bool isConnectPad = false;
	const int DEADZONE = 7849;

public:
	// キーが押されているか
	static bool PressKey(uint8_t keynumber);

	//	キーを押した瞬間
	static bool PushKey(uint8_t keynumber);

	//	キーを離した瞬間
	static bool ReleaseKey(uint8_t keynumber);

	// joystateがつながっているかどうか
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;
	// デッドゾーンの設定
	void SetJoyStickDeadZone(int32_t stickNo, XINPUT_STATE& out) const;


	// パッドに接続されてるか
	bool GetPadConnect() const;

	// パッドのボタンが押されているか
	bool GetPadButton(UINT button) const;

	// パッドのボタンが離れた瞬間か
	bool GetPadButtonUp(UINT button) const;

	// パッドのボタンが押された瞬間か
	bool GetPadButtonDown(UINT button) const;

	// パッドの左スティック
	Vector2 GetPadLStick() const;

	// パッドの右スティック
	Vector2 GetPadRStick() const;

};