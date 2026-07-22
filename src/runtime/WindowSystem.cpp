#include "WindowSystem.h"

#include <SDL3/SDL.h>

namespace shzk
{
	void WindowSystem::Init(const WindowSystemInitInfo& windowSystemInitInfo)
	{
		m_window = SDL_CreateWindow(
			windowSystemInitInfo.title,
			windowSystemInitInfo.width,
			windowSystemInitInfo.height,
			SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);
		m_width = windowSystemInitInfo.width;
		m_height = windowSystemInitInfo.height;
	}

	void WindowSystem::Shutdown()
	{
		SDL_DestroyWindow(m_window);
		m_window = nullptr;
	}

	void WindowSystem::Tick()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_EVENT_QUIT) {
				m_shouldClose = true;
			}
		}
	}

}