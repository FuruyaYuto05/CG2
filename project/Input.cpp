

#include "Input.h"
#include <cassert>


#pragma comment(lib,"dinput8.lib")
#pragma comment(lib,"dxguid.lib")



void Input::Initialize(HINSTANCE hInstance, HWND hwnd)
{

	// DirectInputオブジェクトの生成
	IDirectInput8* directInput = nullptr; // ① グローバル変数（またはWinMain外）として宣言されている場合もあります
	HRESULT result = DirectInput8Create(
		GetModuleHandle(nullptr), // ② WinMainのhInstaceでもOKですが、ここではモジュールハンドルを取得しています
		DIRECTINPUT_VERSION,
		IID_IDirectInput8,
		(void**)&directInput,
		nullptr
	);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	result = keyboard->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(result));

	result = keyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(result));
	
}

void Input::Update()
{

	//キーボード情報の取得開始
	keyboard->Acquire();

	//全キーの入力状態を取得する
	BYTE key[256] = {};
	keyboard->GetDeviceState(sizeof(key), key);

}
