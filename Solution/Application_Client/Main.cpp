#include "stdafx.h"

LRESULT CALLBACK WndProc(HWND aWindowHandler, UINT aMessage, WPARAM aWParam, LPARAM aLParam);
void OnResize();
void CleanUp();
bool ReadSetup(const std::string& aFilePath, VGE::SetupInfo& aOutputSetupInfo);

Game* locGame = nullptr;
CU::Vector2<int> locWindowSize(800, 600);
bool locWindowActive = false;
bool locWindowResizeing = false;
VGE::SetupInfo locSetup;

int WINAPI WinMain(HINSTANCE aInstanceHandler, HINSTANCE, LPTSTR, int)
{
	bool isRunning = true;
	DL_Debug::Debug::Create();
	CU::TimerManager::Create();

	if (ReadSetup("Not used yet", locSetup) == false)
	{
		MessageBox(NULL, "Failed to read setup file.", "Failed startup", MB_ICONERROR);
		return 1;
	}

	HWND hwnd;
	if (VGE::Engine::Create(hwnd, WndProc, locSetup) == false)
	{
		return 1;
	}

	CU::InputManager::GetInstance()->Create(hwnd, aInstanceHandler, 
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND, 
		DISCL_NONEXCLUSIVE | DISCL_FOREGROUND);

	locGame = new Game();
	if (locGame->Initialize() == false)
	{
		MessageBox(NULL, "Failed to initialize the game.", "Failed startup", MB_ICONERROR);
		return 1;
	}

	SetActiveWindow(hwnd);

	float deltaTime = 0;
	MSG message;
	while (isRunning == true)
	{
		if (PeekMessage(&message, NULL, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
			{
				isRunning = false;
				break;
			}
			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			if (locGame->Update(deltaTime) == false)
			{
				isRunning = false;
				break;
			}
			VGE::Engine::GetInstance()->Render();
		}
	}
	CleanUp();
	return 0;
}

LRESULT CALLBACK WndProc(HWND aWindowHandler, UINT aMessage, WPARAM aWParam, LPARAM aLParam)
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (aMessage)
	{
	case WM_PAINT:
		hdc = BeginPaint(aWindowHandler, &ps);
		EndPaint(aWindowHandler, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_ACTIVATE:
		CU::InputManager::GetInstance()->ToggleWindowActive();
		if (locGame != nullptr)
		{
			if (LOWORD(aWParam) == WA_INACTIVE)
			{
				if (locWindowActive == true)
				{
					locWindowActive = false;
					locGame->Pause();
				}
			}
			else
			{
				if (locWindowActive == false)
				{
					locWindowActive = true;
					locGame->Unpause();
				}
			}
		}
		break;
	case WM_SIZE:
		locWindowSize.x = LOWORD(aLParam);
		locWindowSize.y = HIWORD(aLParam);

		if (locWindowSize.x >= locSetup.myScreenWidth)
			locWindowSize.x = locSetup.myScreenWidth;
		if (locWindowSize.y >= locSetup.myScreenHeight)
			locWindowSize.y = locSetup.myScreenHeight;

		if (LOWORD(aWParam) == SIZE_MINIMIZED)
		{
			if (locGame != nullptr)
			{
				locGame->Pause();
			}
		}
		else if (LOWORD(aWParam) == SIZE_MAXIMIZED)
		{
			OnResize();
		}
		else if (LOWORD(aWParam) == SIZE_RESTORED)
		{
			if (locWindowResizeing == false)
			{
				OnResize();
			}
		}
		break;
	case WM_ENTERSIZEMOVE:
		if (locGame != nullptr)
		{
			locGame->Pause();
			locWindowResizeing = true;
		}
		break;
	case WM_EXITSIZEMOVE:
		if (locGame != nullptr)
		{
			locWindowResizeing = false;
			OnResize();
		}
		break;
	default:
		return DefWindowProc(aWindowHandler, aMessage, aWParam, aLParam);
	}
	return 0;
}

void OnResize()
{
	//Pause, resize and unpause
	if (locGame != nullptr)
	{
		if (locWindowActive == true)
		{
			locGame->Unpause();
		}
		locGame->OnResize(locWindowSize.x, locWindowSize.y);
	}
}

void CleanUp()
{
	locGame->Destroy();
	delete locGame;
	locGame = nullptr;

	CU::InputManager::Destroy();
	CU::TimerManager::Destroy();
	VGE::Engine::Destroy();

	DL_Debug::Debug::Destroy();
}

bool ReadSetup(const std::string& aFilePath, VGE::SetupInfo& aOutputSetupInfo)
{
	aFilePath;
	aOutputSetupInfo.myScreenWidth = 800;
	aOutputSetupInfo.myScreenHeight = 600;
	aOutputSetupInfo.myWindowed = true;
	aOutputSetupInfo.myWindowTitle = "TestWindow";
	return true;
}