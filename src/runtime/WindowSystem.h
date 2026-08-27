#pragma once

#include <iostream>

struct SDL_Window;

namespace shzk
{
	struct WindowSystemInitInfo
	{
		uint32_t width = 1920;
		uint32_t height = 1080;
		const char* title = "shzk";
	};

	class WindowSystem
	{
	public:
		WindowSystem() = default;
		~WindowSystem() = default;

		void Init(const WindowSystemInitInfo& windowDesc);
		void Shutdown();

		void Tick();
		bool ShouldClose() const { return m_shouldClose; }

		SDL_Window* GetWindow() const { return m_window; }
		uint32_t GetWidth() const { return m_width; }
		uint32_t GetHeight() const { return m_height; }

	private:
		SDL_Window* m_window = nullptr;
		bool m_shouldClose = false;

		uint32_t m_width = 0;
		uint32_t m_height = 0;
	};
}