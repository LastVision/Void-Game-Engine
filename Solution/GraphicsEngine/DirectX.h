#pragma once
struct ID3D11Debug;
struct ID3D11DepthStencilState;
struct ID3D11DepthStencilView;
struct ID3D11DepthStencilState;
struct ID3D11Device;
struct ID3D11DeviceChild;
struct ID3D11DeviceContext;
struct ID3D11InfoQueue;
struct ID3D11RasterizerState;
struct ID3D11RenderTargetView;
struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct IDXGISwapChain;
struct D3D11_VIEWPORT;

namespace VoidGameEngine
{
	struct SetupInfo;

	class DirectX
	{
	public:
		DirectX(HWND& aWindowHandler, const SetupInfo& aSetupInfo);
		~DirectX();

		void Present(const unsigned int aSyncInterval, const unsigned int aFlag);
		void Clear(const CU::Vector4<float>& aClearColor);

		void OnResize(const CU::Vector2<int>& aWindowSize);
		void SetFullscreen(bool aFullscreenFlag);
		bool IsFullscreen() const;

		void SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName);

		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContex();
		ID3D11DepthStencilView* GetDepthStencil();
		ID3D11RenderTargetView* GetDepthBuffer();
		ID3D11ShaderResourceView* GetBackbufferView();
		ID3D11Texture2D* GetBackbufferTexture();
		ID3D11Texture2D* GetDepthbufferTexture();

		void RestoreViewPort();
		void SetBackBufferAsTarget();

		void EnableZBuffer();
		void DisableZBuffer();

		void EnableWireframe();
		void DisableWireframe();

		void EnableCulling();
		void DisableCulling();
	private:
		void operator=(const DirectX&) = delete;

		void CleanD3D();
		bool D3DSetup();
		bool D3DSwapChainSetup();
		bool D3DBackbufferSetup(int aWidth, int aHeight);
		bool D3DViewPortSetup(int aWidth, int aHeight);
		bool D3DEnabledStencilStateSetup();
		bool D3DDisabledStencilStateSetup();
		bool D3DWireframeRasterizerStateSetup();
		bool D3DSolidRasterizerStateSetup();
		bool D3DNoCullingRasterizerStateSetup();

		HWND& myWindowHandler;
		const SetupInfo& mySetupInfo;

		ID3D11Device* myDevice;
		ID3D11DeviceContext* myContext;
		ID3D11Debug* myDebugInterface;
		ID3D11InfoQueue *myInfoQueue;
		IDXGISwapChain* mySwapChain;

		ID3D11RenderTargetView* myBackbufferRenderTarget;
		ID3D11DepthStencilView* myBackbufferDepthStencil;
		ID3D11ShaderResourceView* myBackbufferShaderResource;
		ID3D11Texture2D* myDepthbufferTexture;
		ID3D11Texture2D* myBackbufferTexture;

		ID3D11DepthStencilState* myEnabledDepthStencilState;
		ID3D11DepthStencilState* myDisabledDepthStencilState;
		ID3D11RasterizerState* mySolidRasterizer;
		ID3D11RasterizerState* myWireframeRasterizer;
		ID3D11RasterizerState* myNoCullingRasterizer;

		D3D11_VIEWPORT* myViewPort;
	};
}

namespace VoidGameEngine
{
	inline ID3D11Device* DirectX::GetDevice()
	{
		DL_ASSERT_EXP(myDevice != nullptr, "DirectX: myDevice is nullptr, HOW?!");
		return myDevice;
	}

	inline ID3D11DeviceContext* DirectX::GetContex()
	{
		DL_ASSERT_EXP(myContext != nullptr, "DirectX: myContex is nullptr, HOW?!");
		return myContext;
	}

	inline ID3D11DepthStencilView* DirectX::GetDepthStencil()
	{
		return myBackbufferDepthStencil;
	}

	inline ID3D11RenderTargetView* DirectX::GetDepthBuffer()
	{
		return myBackbufferRenderTarget;
	}

	inline ID3D11ShaderResourceView* DirectX::GetBackbufferView()
	{
		return myBackbufferShaderResource;
	}

	inline ID3D11Texture2D* DirectX::GetBackbufferTexture()
	{
		return myBackbufferTexture;
	}

	inline ID3D11Texture2D* DirectX::GetDepthbufferTexture()
	{
		return myDepthbufferTexture;
	}
}