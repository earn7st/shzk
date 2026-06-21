#include "SDLWindow.h"
#include <string>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

void SDLWindow::Initialize(const std::string& name, uint32_t width, uint32_t height)
{
    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    window = SDL_CreateWindow(
        "vkR",
        extent.width = width,
        extent.height = height,
        window_flags );
}

