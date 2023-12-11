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

class KeyInput {
public:
	KeyInput() = default;
	~KeyInput() = default;

	
	static KeyInput* GetInstance();

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
	static bool GetKey(uint8_t keynumber);

	//	キーを押した瞬間
	static bool PushKey(uint8_t keynumber);

	//	キーを離した瞬間
	static bool ReleaseKey(uint8_t keynumber);

	// joystateがつながっているかどうか
	bool GetJoystickState(int32_t stickNo, XINPUT_STATE& out) const;
	// デッドゾーンの設定
	void SetJoyStickDeadZone(int32_t stickNo, XINPUT_STATE& out) const;


	// パッドに接続されてるか
	static bool GetPadConnect();

	// パッドのボタンが押されているか
	static bool GetPadButton(UINT button);

	// パッドのボタンが離れた瞬間か
	static bool GetPadButtonUp(UINT button);

	// パッドのボタンが押された瞬間か
	static bool GetPadButtonDown(UINT button);

	// パッドの左スティック
	Vector2 GetPadLStick();

	// パッドの右スティック
	Vector2 GetPadRStick();

};