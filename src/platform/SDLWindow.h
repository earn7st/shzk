#pragma once

#include <stdint.h>

struct WindowExtent
{
	uint32_t width;
	uint32_t height;
};

struct SDLWindow
{
	struct SDL_Window* _window {nullptr};
	WindowExtent _extent{1920, 1080};
	const char* _name = "vkR";

public:
	void Init();

};