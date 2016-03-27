#pragma once

namespace VoidGameEngine
{
	struct SetupInfo;

	class Engine
	{
	public:
		static bool Create(HWND& aWindowHandler, WNDPROC aWndProc, const SetupInfo& aSetupInfo);
		static void Destroy();
		static Engine* GetInstance();

		void Render();
		void OnResize(const CU::Vector2<int>& aWindowSize);

		const CU::Vector2<int>& GetWindowSize() const;
	private:
		Engine();
		~Engine();

		bool Initialize(HWND& aWindowHandler, WNDPROC aWndProc);
		bool WindowSetup(HWND& aWindowHandler, WNDPROC aWndProc);

		static Engine* myInstance;

		CU::Vector2<int> myWindowSize;

		const SetupInfo* mySetupInfo;
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
};