#include "SDLWindow.h"

#include "VKCommon.h"

void SDLWindow::Init()
{
    // We initialize SDL and create a window with it.
    SDL_Init(SDL_INIT_VIDEO);

    SDL_WindowFlags window_flags = (SDL_WindowFlags)(SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE);

    _window = SDL_CreateWindow(
        "vkR",
        _extent.width,
        _extent.height,
        window_flags );
}

