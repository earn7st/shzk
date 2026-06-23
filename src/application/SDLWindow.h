#pragma once

#include <stdint.h>
#include <string>

struct WindowExtent
{
	uint32_t width;
	uint32_t height;
};

struct SDLWindow
{
	struct SDL_Window* window {nullptr};
	WindowExtent extent;
	const char* name = "vkR";

public:
	SDLWindow() = default;
	SDLWindow(const std::string& name, uint32_t width = 1920, uint32_t height = 1080);

	~SDLWindow() = default;
};