#include "stdafx.h"
#include "Engine.h"
#include "SetupInfo.h"
#include <Windows.h>

namespace VoidGameEngine
{
	Engine* Engine::myInstance = nullptr;

	Engine::Engine()
	{}

	Engine::~Engine()
	{}

	bool Engine::Create(HWND& aWindowHandler, WNDPROC aWndProc, const SetupInfo& aSetupInfo)
	{
		DL_ASSERT_EXP(myInstance == nullptr, "Can't create engine twice.");
		myInstance = new Engine();
		myInstance->mySetupInfo = &aSetupInfo;

		bool result = myInstance->Initialize(aWindowHandler, aWndProc);

		//TODO: Enable fullscreen here when supported

		myInstance->Render();

		return result;
	}

	void Engine::Destroy()
	{
		delete myInstance;
		myInstance = nullptr;
	}

	void Engine::Render()
	{
		//TODO: Handle Rendering
	}

	void Engine::OnResize(const CU::Vector2<int>& aWindowSize)
	{
		myWindowSize = aWindowSize;
		//TODO: HandleResize here
	}

	bool Engine::Initialize(HWND& aWindowHandler, WNDPROC aWndProc)
	{
		myWindowSize.x = mySetupInfo->myScreenWidth;
		myWindowSize.y = mySetupInfo->myScreenHeight;

		if (WindowSetup(aWindowHandler, aWndProc) == false)
		{
			ENGINE_LOG("Failed to Create Window");
			return false;
		}

		//TODO: Setup DirectX later enable OpenGL support

		ShowWindow(aWindowHandler, 10);
		UpdateWindow(aWindowHandler);

		ENGINE_LOG("Engine Init Successful");
		return true;
	}

	bool Engine::WindowSetup(HWND& aWindowHandler, WNDPROC aWndProc)
	{
		WNDCLASSEX wcex;

		wcex.cbSize = sizeof(WNDCLASSEX);

		wcex.style = CS_HREDRAW | CS_VREDRAW;
		wcex.lpfnWndProc = aWndProc;
		wcex.cbClsExtra = 0;
		wcex.cbWndExtra = 0;
		wcex.hInstance = GetModuleHandle(NULL);
		wcex.hIcon = LoadIcon(GetModuleHandle(NULL), NULL);
		wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
		wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
		wcex.lpszMenuName = NULL;
		wcex.lpszClassName = mySetupInfo->myWindowTitle.c_str();
		wcex.hIconSm = LoadIcon(wcex.hInstance, NULL);

		if (RegisterClassEx(&wcex) == false)
		{
			ENGINE_LOG("Failed to register WindowClass");
			return false;
		}

		RECT rc = { 0, 0, mySetupInfo->myScreenWidth, mySetupInfo->myScreenHeight };
		AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

		aWindowHandler = CreateWindowEx(
			WS_EX_CLIENTEDGE,
			mySetupInfo->myWindowTitle.c_str(),
			mySetupInfo->myWindowTitle.c_str(),
			WS_OVERLAPPEDWINDOW,
			0,
			0,
			rc.right - rc.left,
			rc.bottom - rc.top,
			NULL,
			NULL,
			GetModuleHandle(NULL),
			NULL);

		if (!aWindowHandler)
		{
			ENGINE_LOG("Failed to CreateWindow");
			return false;
		}

		ENGINE_LOG("Window Setup Successful");
		return true;
	}
}