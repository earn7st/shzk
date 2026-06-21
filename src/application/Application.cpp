#include "application/Application.h"

#include <SDL3/SDL.h>

void Application::Initialize(const std::string& name, uint32_t width, uint32_t height)
{
	window.Initialize(name, width, height);
	renderer.Initialize(window);

}

void Application::Run()
{
	SDL_Event e;
	bool bWindowShouldClose = false;
	while (!bWindowShouldClose)
	{
		while (SDL_PollEvent(&e) != 0) {
			if (e.type == SDL_EVENT_QUIT) bWindowShouldClose = true;
		}

		this->Tick(deltaTime);
	}
	
}

void Application::Tick(double dt)
{

}

void Application::Shutdown()
{

}
