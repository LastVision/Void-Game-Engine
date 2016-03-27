#include "stdafx.h"
#include "InputManager.h"

CU::InputManager* CU::InputManager::myInstance = nullptr;

void CU::InputManager::Create(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags)
{
	assert(myInstance == nullptr && "InputManager already created!");
	if (myInstance == nullptr)
	{
		myInstance = new CU::InputManager();
		myInstance->Init(aHwnd, aHInstance, aKeyCoopFlags, aMouseCoopFlags);
	}
}

CU::InputManager* CU::InputManager::GetInstance()
{
	return myInstance;
}

void CU::InputManager::Destroy()
{
	delete myInstance;
	myInstance = nullptr;
}

CU::InputManager::InputManager()
	: myWindowIsActive(true)
{

}

void CU::InputManager::Init(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags)
{

	DirectInput8Create(aHInstance, DIRECTINPUT_VERSION, IID_IDirectInput8, reinterpret_cast<void**>(&myDirectInput), 0);

	myDirectInput->CreateDevice(GUID_SysKeyboard, &myKeyboardDevice, nullptr);
	myDirectInput->CreateDevice(GUID_SysMouse, &myMouseDevice, nullptr);

	myKeyboardDevice->SetDataFormat(&c_dfDIKeyboard);
	myMouseDevice->SetDataFormat(&c_dfDIMouse);

	myKeyboardDevice->SetCooperativeLevel(aHwnd, aKeyCoopFlags);
	myMouseDevice->SetCooperativeLevel(aHwnd, aMouseCoopFlags);

	myKeyboardDevice->Acquire();
	myMouseDevice->Acquire();

	myWindowHandler = aHwnd;

	myIsRecordingDeltas = true;

	Update();
}

CU::InputManager::~InputManager()
{
	myKeyboardDevice->Unacquire();
	myMouseDevice->Unacquire();
}

void CU::InputManager::Update()
{
	if (myWindowIsActive == true)
	{
		CapturePreviousState();
		HRESULT hr = myKeyboardDevice->GetDeviceState(sizeof(myKeyState), reinterpret_cast<void**>(&myKeyState));

		if (FAILED(hr))
		{
			ZeroMemory(myKeyState, sizeof(myKeyState));

			myKeyboardDevice->Acquire();
		}

		hr = myMouseDevice->GetDeviceState(sizeof(DIMOUSESTATE), reinterpret_cast<void**>(&myMouseState));
		if (FAILED(hr))
		{
			ZeroMemory(&myMouseState, sizeof(myMouseState));

			myMouseDevice->Acquire();
		}

		tagPOINT cursorPoint;
		GetCursorPos(&cursorPoint);
		ScreenToClient(myWindowHandler, &cursorPoint);
		myMousePos.x = static_cast<float>(cursorPoint.x);
		myMousePos.y = static_cast<float>(cursorPoint.y);

		if (myIsRecordingDeltas == false)
		{
			myMouseState.lX = 0;
			myMouseState.lY = 0;
		}
	}
}

void CU::InputManager::ToggleWindowActive()
{
	if (myInstance != nullptr)
	{
		myWindowIsActive = !myWindowIsActive;
	}
}


void CU::InputManager::PauseDeltaRecording()
{
	myIsRecordingDeltas = false;
}


void CU::InputManager::ResumeDeltaRecording()
{
	myIsRecordingDeltas = true;
}

const CU::Vector2<float>& CU::InputManager::GetMousePosition() const
{
	return myMousePos;
}

void CU::InputManager::CapturePreviousState()
{
	memcpy_s(myPreviousKeyState, sizeof(myPreviousKeyState), myKeyState, sizeof(myKeyState));
	memcpy_s(&myPreviousMouseState, sizeof(myPreviousMouseState), &myMouseState, sizeof(myMouseState));
}

bool CU::InputManager::KeyIsPressed(unsigned int aKey) const
{
	UCHAR key = static_cast<UCHAR>(aKey);
	return ((myKeyState[key] & 0x80) != 0 && (myPreviousKeyState[key] & 0x80) == 0);
}

bool CU::InputManager::KeyDown(unsigned int aKey) const
{
	UCHAR key = static_cast<UCHAR>(aKey);
	return (myKeyState[key] & 0x80) != 0;
}

bool CU::InputManager::KeyUp(unsigned int aKey) const
{
	UCHAR key = static_cast<UCHAR>(aKey);
	return ((myKeyState[key] & 0x80) == 0 && (myPreviousKeyState[key] & 0x80) != 0);
}

bool CU::InputManager::MouseIsPressed(int aButton) const
{
	return ((myMouseState.rgbButtons[aButton] & 0x80) != 0 && (myPreviousMouseState.rgbButtons[aButton] & 0x80) == 0);
}

bool CU::InputManager::MouseDown(int aButton) const
{
	return (myMouseState.rgbButtons[aButton] & 0x80) != 0;
}

bool CU::InputManager::MouseUp(int aButton) const
{
	return ((myMouseState.rgbButtons[aButton] & 0x80) == 0 && (myPreviousMouseState.rgbButtons[aButton] & 0x80) != 0);
}

float CU::InputManager::GetMouseDX() const
{
	return static_cast<float>(myMouseState.lX);
}

float CU::InputManager::GetMouseDY() const
{
	return static_cast<float>(myMouseState.lY);
}

float CU::InputManager::GetMouseDZ() const
{
	return static_cast<float>(myMouseState.lZ);
}