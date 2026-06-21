#include <iostream>
#include "application/Application.h"

int main()
{
    try
    {
        SDLWindow window;

        window.Initialize(
            "Vulkan Initialization Test",
            1280,
            720);

        Renderer renderer;

        renderer.Initialize(window);

        std::cout << "Vulkan initialization succeeded!\n";

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr
            << "Initialization failed:\n"
            << e.what()
            << std::endl;

        return -1;
    }

	return 0;
}