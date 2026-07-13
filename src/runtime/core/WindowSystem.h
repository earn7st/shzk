#pragma once

#include <string>

struct SDL_Window;

namespace vkR
{
	struct WindowSpec
	{
		uint32_t width = 1920;
		uint32_t height = 1080;
		std::string title = "vkR";
	};

	class WindowSystem
	{
	public:
		WindowSystem() = default;
		~WindowSystem() = default;

		void Initialize(const WindowSpec& windowSpec);
		void Tick();
		void Shutdown();

		SDL_Window* GetWindow() const { return m_window; }

		bool ShouldClose() const { return m_shouldClose; }

	private:
		SDL_Window* m_window = nullptr;

		bool m_shouldClose = false;
	};
}