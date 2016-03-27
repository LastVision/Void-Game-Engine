#include "stdafx.h"

#include "DirectX.h"
#include "Engine.h"
#include "SetupInfo.h"
#include <Windows.h>

namespace VoidGameEngine
{
	Engine* Engine::myInstance = nullptr;

	Engine::Engine()
		: myClearColor({ 0.8f, 0.125f, 0.8f, 1.0f })
		, myWireframeIsOn(false)
		, myWireframeShouldShow(false)
	{}

	Engine::~Engine()
	{
		delete myDirectX;
		myDirectX = nullptr;
	}

	bool Engine::Create(HWND& aWindowHandler, WNDPROC aWndProc, const SetupInfo& aSetupInfo)
	{
		DL_ASSERT_EXP(myInstance == nullptr, "Can't create engine twice.");
		myInstance = new Engine();
		myInstance->mySetupInfo = &aSetupInfo;

		bool result = myInstance->Initialize(aWindowHandler, aWndProc);

		if (aSetupInfo.myWindowed == false)
		{
			myInstance->myDirectX->SetFullscreen(true);
		}

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
		myDirectX->Present(0, 0);
		myDirectX->Clear(myClearColor);
	}

	void Engine::OnResize(const CU::Vector2<int>& aWindowSize)
	{
		myWindowSize = aWindowSize;
		
		if (myDirectX != nullptr)
		{
			myDirectX->OnResize(aWindowSize);
		}
	}

	const bool Engine::IsFullscreen() const
	{
		return myDirectX->IsFullscreen();
	}

	void Engine::SetFullscreen(const bool aFullscreenFlag)
	{
		myDirectX->SetFullscreen(aFullscreenFlag);
	}

	void Engine::SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName)
	{
		myDirectX->SetDebugName(aChild, aName);
	}

	ID3D11Device* Engine::GetDevice()
	{
		return myDirectX->GetDevice();
	}

	ID3D11DeviceContext* Engine::GetContex()
	{
		return myDirectX->GetContex();
	}

	ID3D11DepthStencilView* Engine::GetDepthView()
	{
		return myDirectX->GetDepthStencil();
	}

	ID3D11RenderTargetView* Engine::GetDepthBuffer()
	{
		return myDirectX->GetDepthBuffer();
	}

	ID3D11ShaderResourceView* Engine::GetBackbufferView()
	{
		return myDirectX->GetBackbufferView();
	}

	ID3D11Texture2D* Engine::GetDepthBufferTexture()
	{
		return myDirectX->GetDepthbufferTexture();
	}

	void Engine::RestoreViewPort()
	{
		myDirectX->RestoreViewPort();
	}

	void Engine::SetBackBufferAsTarget()
	{
		myDirectX->SetBackBufferAsTarget();
	}

	void Engine::EnableZBuffer()
	{
		myDirectX->EnableZBuffer();
	}

	void Engine::DisableZBuffer()
	{
		myDirectX->DisableZBuffer();
	}

	void Engine::ToggleWireframe()
	{
		myDirectX->EnableWireframe();


		if (myWireframeIsOn == true)
		{
			myDirectX->DisableWireframe();
			myWireframeIsOn = false;
			myWireframeShouldShow = false;
			return;
		}

		myWireframeShouldShow = true;
		myWireframeIsOn = true;
	}

	void Engine::EnableWireframe()
	{
		myDirectX->EnableWireframe();
	}

	void Engine::DisableWireframe()
	{
		myDirectX->DisableWireframe();
	}

	void Engine::EnableCulling()
	{
		myDirectX->EnableCulling();
	}

	void Engine::DisableCulling()
	{
		myDirectX->DisableCulling();
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

		myDirectX = new DirectX(aWindowHandler, *mySetupInfo);
		if (myDirectX == nullptr)
		{
			ENGINE_LOG("Failed to Setup DirectX");
			return false;
		}

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