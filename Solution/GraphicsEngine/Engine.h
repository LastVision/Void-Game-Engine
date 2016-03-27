#pragma once

struct ID3D11Device;
struct ID3D11DeviceChild;
struct ID3D11DeviceContext;
struct ID3D11DepthStencilView;
struct ID3D11RenderTargetView;
struct ID3D11ShaderResourceView;
struct ID3D11Texture2D;

namespace VoidGameEngine
{
	class DirectX;

	struct SetupInfo;

	class Engine
	{
	public:
		static bool Create(HWND& aWindowHandler, WNDPROC aWndProc, const SetupInfo& aSetupInfo);
		static void Destroy();
		static Engine* GetInstance();

		void Render();
		void OnResize(const CU::Vector2<int>& aWindowSize);
		const bool IsFullscreen() const;
		void SetFullscreen(const bool aFullscreenFlag);

		void SetDebugName(ID3D11DeviceChild* aChild, const std::string& aName);
		
		ID3D11Device* GetDevice();
		ID3D11DeviceContext* GetContex();
		ID3D11DepthStencilView* GetDepthView();
		ID3D11RenderTargetView* GetDepthBuffer();
		ID3D11ShaderResourceView* GetBackbufferView();
		ID3D11Texture2D* GetDepthBufferTexture();

		const CU::Vector2<int>& GetWindowSize() const;

		void RestoreViewPort();
		void SetBackBufferAsTarget();

		void EnableZBuffer();
		void DisableZBuffer();

		void ToggleWireframe();

		void EnableWireframe();
		void DisableWireframe();

		void EnableCulling();
		void DisableCulling();

		void SetClearColor(const CU::Vector4<float>& aClearColor);
	private:
		Engine();
		~Engine();

		bool Initialize(HWND& aWindowHandler, WNDPROC aWndProc);
		bool WindowSetup(HWND& aWindowHandler, WNDPROC aWndProc);

		static Engine* myInstance;

		DirectX* myDirectX;

		CU::Vector2<int> myWindowSize;
		CU::Vector4<float> myClearColor;

		const SetupInfo* mySetupInfo;

		bool myWireframeIsOn;
		bool myWireframeShouldShow;
	};
};


namespace VoidGameEngine
{
	inline Engine* Engine::GetInstance()
	{
		return myInstance;
	}

	inline const CU::Vector2<int>& Engine::GetWindowSize() const
	{
		return myWindowSize;
	}

	inline void Engine::SetClearColor(const CU::Vector4<float>& aClearColor)
	{
		myClearColor = aClearColor;
	}
};