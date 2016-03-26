#pragma once
#pragma comment(lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <Vector.h>


namespace CU
{
	class InputManager
	{
	public:
		UCHAR myKeyState[256];

		static void Create(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags);
		static void Destroy();
		static InputManager* GetInstance();

		float GetMouseDX() const;
		float GetMouseDY() const;
		float GetMouseDZ() const;

		const CU::Vector2<float>& GetMousePosition() const;

		bool MouseDown(int aButton) const; 
		bool MouseUp(int aButton) const;
		bool MouseIsPressed(int aButton) const; 

		bool KeyDown(unsigned int aKey) const;
		bool KeyUp(unsigned int aKey) const;
		bool KeyIsPressed(unsigned int aKey) const;

		void Update();
		void PauseDeltaRecording();
		void ResumeDeltaRecording();
		void ToggleWindowActive();
	private:
		InputManager();
		~InputManager();
		void Init(HWND aHwnd, HINSTANCE aHInstance, DWORD aKeyCoopFlags, DWORD aMouseCoopFlags);
		void CapturePreviousState();

		static InputManager* myInstance;

		LPDIRECTINPUT8 myDirectInput;
		LPDIRECTINPUTDEVICE8 myKeyboardDevice;

		UCHAR myPreviousKeyState[256];

		LPDIRECTINPUTDEVICE8 myMouseDevice;
		DIMOUSESTATE myMouseState;
		DIMOUSESTATE myPreviousMouseState;

		CU::Vector2<float> myMousePos;

		HWND myWindowHandler;

		bool myIsRecordingDeltas;
		bool myWindowIsActive;
	};
}