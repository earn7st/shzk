#include "WindowSystem.h"
#include "runtime/log/Log.h"

#include <SDL3/SDL.h>

namespace shzk
{
	void WindowSystem::Init(const WindowSystemInitInfo& windowSystemInitInfo)
	{
		SDL_Init(SDL_INIT_VIDEO);

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
		
	}

}