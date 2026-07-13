#include "WindowSystem.h"

#include <iostream>

#include <SDL3/SDL.h>

namespace vkR
{
	void WindowSystem::Initialize(const WindowSpec& windowSpec)
	{
		SDL_Init(SDL_INIT_VIDEO);
		m_window = SDL_CreateWindow(
			windowSpec.title.c_str(), 
			windowSpec.width, 
			windowSpec.height, 
			SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

		if (!m_window)
		{
			std::cerr << "[Debug] WindowSystem::Initialize(): SDL_CreateWindow Failed!" << SDL_GetError() << std::endl;
			SDL_Quit();
			return;
		}
		
	}

	void WindowSystem::Tick()
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_EVENT_QUIT)
			{
				m_shouldClose = true;
			}
		}
	}

	void WindowSystem::Shutdown()
	{
		SDL_DestroyWindow(m_window);
		SDL_QuitSubSystem(SDL_INIT_VIDEO);
		SDL_Quit();
	}
}