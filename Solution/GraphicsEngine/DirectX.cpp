#include "stdafx.h"
#include "DirectX.h"
#include <d3d11.h>
#include "SetupInfo.h"

namespace VoidGameEngine
{
	DirectX::DirectX(HWND& aWindowHandler, const SetupInfo& aSetupInfo)
		: myWindowHandler(aWindowHandler)
		, mySetupInfo(aSetupInfo)
	{
		D3DSetup();
	}

	DirectX::~DirectX()
	{
		CleanD3D();
	}

	void DirectX::Present(const unsigned int aSyncInterval, const unsigned int aFlags)
	{
		mySwapChain->Present(aSyncInterval, aFlags);
	}

	void DirectX::Clear(const CU::Vector4<float>& aClearColor)
	{
		float clearColor[4] = { aClearColor.x, aClearColor.y, aClearColor.z, aClearColor.w };
		myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, myBackbufferDepthStencil);
		myContext->ClearRenderTargetView(myBackbufferRenderTarget, clearColor);
		myContext->ClearDepthStencilView(myBackbufferDepthStencil, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	}

	void DirectX::OnResize(const CU::Vector2<int>& aWindowSize)
	{
		myContext->OMSetRenderTargets(0, NULL, NULL);
		myBackbufferRenderTarget->Release();
		myBackbufferShaderResource->Release();
		myBackbufferTexture->Release();
		myContext->Flush();

		HRESULT result = mySwapChain->ResizeBuffers(1, aWindowSize.x, aWindowSize.y, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
		if (result != S_OK)
		{
			DIRECTX_LOG("Failed to Resize SwapChain Buffers.");
			DL_MESSAGE_BOX("Failed to resize swapchain buffers.", "DirectX: SwapChain", MB_ICONERROR);
		}

		D3DBackbufferSetup(aWindowSize.x, aWindowSize.y);
		D3DViewPortSetup(aWindowSize.x, aWindowSize.y);
	}

	void DirectX::SetFullscreen(bool aFullscreenFlag)
	{
		HRESULT result = mySwapChain->SetFullscreenState(aFullscreenFlag, nullptr);
		if (result != S_OK)
		{
			DIRECTX_LOG("Failed to SetFullscreenState on SwapChain Buffers");
			DL_MESSAGE_BOX("Failed to SetFullscreenState on SwapChain Buffers", "DirectX: SwapChain", MB_ICONWARNING);
		}
	}

	bool DirectX::IsFullscreen() const
	{
		BOOL isFullscreen;
		HRESULT result = mySwapChain->GetFullscreenState(&isFullscreen, nullptr);
		if (result != S_OK)
		{
			DIRECTX_LOG("Failed to GetFullscreenState on SwapChain Buffers");
			DL_MESSAGE_BOX("Failed to GetFullscreenState on SwapChain Buffers", "DirectX: SwapChain", MB_ICONWARNING);
		}
		return isFullscreen == TRUE;
	}

	void DirectX::SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName)
	{
		if (aChild != nullptr)
		{
			aChild->SetPrivateData(WKPDID_D3DDebugObjectName, aName.size(), aName.c_str());
		}
	}

	void DirectX::RestoreViewPort()
	{
		myContext->RSSetViewports(1, myViewPort);
	}

	void DirectX::SetBackBufferAsTarget()
	{
		myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, myBackbufferDepthStencil);
	}
		 
	void DirectX::EnableZBuffer()
	{
		myContext->OMSetDepthStencilState(myEnabledDepthStencilState, 1);
	}

	void DirectX::DisableZBuffer()
	{
		myContext->OMSetDepthStencilState(myDisabledDepthStencilState, 1);
	}
		
	void DirectX::EnableWireframe()
	{
		myContext->RSSetState(myWireframeRasterizer);
	}

	void DirectX::DisableWireframe()
	{
		myContext->RSSetState(mySolidRasterizer);
	}
		
	void DirectX::EnableCulling()
	{
		myContext->RSSetState(mySolidRasterizer);
	}

	void DirectX::DisableCulling()
	{
		myContext->RSSetState(myNoCullingRasterizer);
	}

	void DirectX::CleanD3D()
	{
		mySwapChain->SetFullscreenState(FALSE, NULL);

		mySwapChain->Release();
		mySwapChain = nullptr;

		myBackbufferRenderTarget->Release();
		myBackbufferRenderTarget = nullptr;

		myBackbufferShaderResource->Release();
		myBackbufferShaderResource = nullptr;

		myBackbufferTexture->Release();
		myBackbufferTexture = nullptr;

		myBackbufferDepthStencil->Release();
		myBackbufferDepthStencil = nullptr;

		myDevice->Release();
		myDevice = nullptr;

		myEnabledDepthStencilState->Release();
		myEnabledDepthStencilState = nullptr;

		myDisabledDepthStencilState->Release();
		myDisabledDepthStencilState = nullptr;

		mySolidRasterizer->Release();
		mySolidRasterizer = nullptr;

		myWireframeRasterizer->Release();
		myWireframeRasterizer = nullptr;

		delete myViewPort;
		myViewPort = nullptr;

		myContext->ClearState();
		myContext->Flush();
		myContext->Release();
		myContext = nullptr;
		//TODO: Fix directX debug interface
		/*
#ifdef _DEBUG
		myDebugInterface->ReportLiveDeviceObjects(D3D11_RLDO_DETAIL);
		myDebugInterface->Release();
#endif*/
	}

	bool DirectX::D3DSetup()
	{
		if (D3DSwapChainSetup() == false)
		{
			DIRECTX_LOG("Failed to Setup DirectX SwapChain");
			return false;
		}
		if (D3DBackbufferSetup(mySetupInfo.myScreenWidth, mySetupInfo.myScreenHeight) == false)
		{
			DIRECTX_LOG("Failed to Setup Backbuffer");
			return false;
		}
		if (D3DViewPortSetup(mySetupInfo.myScreenWidth, mySetupInfo.myScreenHeight) == false)
		{
			DIRECTX_LOG("Failed to Setup DirectX ViewPort");
			return false;
		}
		if (D3DEnabledStencilStateSetup() == false)
		{
			DIRECTX_LOG("Failed to Setup EnabledStencilBuffer");
			return false;
		}
		if (D3DDisabledStencilStateSetup() == false)
		{
			DIRECTX_LOG("Failed to Setup DisabledStencilBuffer");
			return false;
		}
		if (D3DWireframeRasterizerStateSetup() == false)
		{
			DIRECTX_LOG("Failed to Setup WireframeRasterizerState");
			return false;
		}
		if (D3DSolidRasterizerStateSetup() == false)
		{
			DIRECTX_LOG("Failed to Setup SolidRasterizerState");
			return false;
		}
		if (D3DNoCullingRasterizerStateSetup() == false)
		{
			DIRECTX_LOG("Failed to Setup SolidRasterizerState");
			return false;
		}
		DisableWireframe();
		EnableZBuffer();

		DIRECTX_LOG("DirectX Setup Successful");
		return true;
	}

	bool DirectX::D3DSwapChainSetup()
	{
		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		ZeroMemory(&swapChainDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

		swapChainDesc.BufferCount = 1;
		swapChainDesc.BufferDesc.Width = mySetupInfo.myScreenWidth;
		swapChainDesc.BufferDesc.Height = mySetupInfo.myScreenHeight;
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_SHADER_INPUT;
		swapChainDesc.OutputWindow = myWindowHandler;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.Windowed = true;

		HRESULT result = D3D11CreateDeviceAndSwapChain(NULL,
			D3D_DRIVER_TYPE_HARDWARE,
			NULL,
			NULL,
			NULL,
			NULL,
			D3D11_SDK_VERSION,
			&swapChainDesc,
			&mySwapChain,
			&myDevice,
			NULL,
			&myContext);

		if (FAILED(result))
		{
			return false;
		}
		//TODO: Fix directX debug info
		/*
#ifdef _DEBUG
		myDebugInterface = nullptr;
		result = myDevice->QueryInterface(__uuidof(ID3D11Debug), (void**)&myDebugInterface);
		if (FAILED(result))
		{
			DL_ASSERT("[DirectX]: Failed to Query DebugInterface");
			return false;
		}

		myInfoQueue = nullptr;
		if (FAILED(myDebugInterface->QueryInterface(__uuidof(ID3D11InfoQueue), (void**)&myInfoQueue)))
		{
			DL_ASSERT("[DirectX]: Failed to Query InfoQueue");
			return false;
		}

		myInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_CORRUPTION, true);
		myInfoQueue->SetBreakOnSeverity(D3D11_MESSAGE_SEVERITY_ERROR, true);

		D3D11_MESSAGE_ID hide[] =
		{
			D3D11_MESSAGE_ID_DEVICE_PSSETSHADERRESOURCES_HAZARD,
			D3D11_MESSAGE_ID_DEVICE_OMSETRENDERTARGETS_HAZARD
			// Add more message IDs here as needed
		};

		D3D11_INFO_QUEUE_FILTER filter;
		memset(&filter, 0, sizeof(filter));
		filter.DenyList.NumIDs = _countof(hide);
		filter.DenyList.pIDList = hide;
		myInfoQueue->AddStorageFilterEntries(&filter);
		myInfoQueue->Release();
#endif
		*/
		return true;
	}

	bool DirectX::D3DBackbufferSetup(int aWidth, int aHeight)
	{
		mySwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&myBackbufferTexture);

		myDevice->CreateRenderTargetView(myBackbufferTexture, NULL, &myBackbufferRenderTarget);
		myDevice->CreateShaderResourceView(myBackbufferTexture, NULL, &myBackbufferShaderResource);

		myContext->OMSetRenderTargets(1, &myBackbufferRenderTarget, NULL);

		HRESULT hr = S_OK;
		D3D11_TEXTURE2D_DESC depthBufferInfo;
		ZeroMemory(&depthBufferInfo, sizeof(depthBufferInfo));

		depthBufferInfo.Width = aWidth;
		depthBufferInfo.Height = aHeight;
		depthBufferInfo.MipLevels = 1;
		depthBufferInfo.ArraySize = 1;
		depthBufferInfo.Format = DXGI_FORMAT_D32_FLOAT;
		depthBufferInfo.SampleDesc.Count = 1;
		depthBufferInfo.Usage = D3D11_USAGE_DEFAULT;
		depthBufferInfo.BindFlags = D3D11_BIND_DEPTH_STENCIL;

		hr = myDevice->CreateTexture2D(&depthBufferInfo, NULL, &myDepthbufferTexture);
		if (FAILED(hr))
		{
			return false;
		}
		D3D11_DEPTH_STENCIL_VIEW_DESC stencilDesc;
		ZeroMemory(&stencilDesc, sizeof(stencilDesc));

		stencilDesc.Format = depthBufferInfo.Format;
		stencilDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		stencilDesc.Texture2D.MipSlice = 0;

		hr = myDevice->CreateDepthStencilView(myDepthbufferTexture, &stencilDesc, &myBackbufferDepthStencil);
		if (FAILED(hr))
		{
			return false;
		}
		return true;
	}

	bool DirectX::D3DViewPortSetup(int aWidth, int aHeight)
	{
		delete myViewPort;
		myViewPort = new D3D11_VIEWPORT();
		ZeroMemory(myViewPort, sizeof(D3D11_VIEWPORT));

		myViewPort->TopLeftX = 0;
		myViewPort->TopLeftY = 0;
		myViewPort->Width = static_cast<float>(aWidth);
		myViewPort->Height = static_cast<float>(aHeight);
		myViewPort->MinDepth = 0.f;
		myViewPort->MaxDepth = 1.f;

		myContext->RSSetViewports(1, myViewPort);

		return true;
	}

	bool DirectX::D3DEnabledStencilStateSetup()
	{
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC  stencilDesc;
		ZeroMemory(&stencilDesc, sizeof(stencilDesc));

		stencilDesc.DepthEnable = true;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stencilDesc.StencilEnable = true;
		stencilDesc.StencilReadMask = 0xFF;
		stencilDesc.StencilWriteMask = 0xFF;
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = myDevice->CreateDepthStencilState(&stencilDesc, &myEnabledDepthStencilState);
		if (FAILED(hr))
		{
			return false;
		}
		return true;
	}

	bool DirectX::D3DDisabledStencilStateSetup()
	{
		HRESULT hr = S_OK;
		D3D11_DEPTH_STENCIL_DESC  stencilDesc;
		ZeroMemory(&stencilDesc, sizeof(stencilDesc));

		stencilDesc.DepthEnable = false;
		stencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		stencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
		stencilDesc.StencilEnable = true;
		stencilDesc.StencilReadMask = 0xFF;
		stencilDesc.StencilWriteMask = 0xFF;
		stencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		stencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		stencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		stencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		stencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

		hr = myDevice->CreateDepthStencilState(&stencilDesc, &myDisabledDepthStencilState);
		if (FAILED(hr))
		{
			return false;
		}
		return true;
	}

	bool DirectX::D3DWireframeRasterizerStateSetup()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

		desc.FillMode = D3D11_FILL_WIREFRAME;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.FrontCounterClockwise = true;
		desc.DepthBias = false;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		myDevice->CreateRasterizerState(&desc, &myWireframeRasterizer);
		return true;
	}

	bool DirectX::D3DSolidRasterizerStateSetup()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_FRONT;
		desc.FrontCounterClockwise = true;
		desc.DepthBias = false;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		myDevice->CreateRasterizerState(&desc, &mySolidRasterizer);
		return true;
	}

	bool DirectX::D3DNoCullingRasterizerStateSetup()
	{
		D3D11_RASTERIZER_DESC desc;
		ZeroMemory(&desc, sizeof(D3D11_RASTERIZER_DESC));

		desc.FillMode = D3D11_FILL_SOLID;
		desc.CullMode = D3D11_CULL_NONE;
		desc.FrontCounterClockwise = true;
		desc.DepthBias = false;
		desc.DepthBiasClamp = 0;
		desc.SlopeScaledDepthBias = 0;
		desc.DepthClipEnable = false;
		desc.ScissorEnable = false;
		desc.MultisampleEnable = false;
		desc.AntialiasedLineEnable = false;

		myDevice->CreateRasterizerState(&desc, &myNoCullingRasterizer);
		return true;
	}

}