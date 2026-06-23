#include "application/Application.h"

#include <SDL3/SDL.h>

Application::Application(const std::string& name, uint32_t width, uint32_t height) :
	window(name, width, height), renderer(window) 
{ 
}

void Application::Initialize()
{
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
