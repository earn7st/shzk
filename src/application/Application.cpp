#include "application/Application.h"

#include <SDL3/SDL.h>
#include <fmt/core.h>

#include "gltf/gltfLoader.h"

Application::Application(const std::string& name, uint32_t width, uint32_t height) :
	window(name, width, height), renderer(window), sceneRenderer(&scene, &resourceManager)
{
}

void Application::Init()
{
    gltfLoader loader(
        renderer.GetVulkanContext(),
        renderer.GetUpladContext(),
        &resourceManager);

    bool ok = loader.LoadGltf(&scene, "C:/Users/earn/workspace/glTF-Sample-Assets/Models/CesiumMilkTruck/glTF/CesiumMilkTruck.gltf");
    if (!ok)
    {
        fmt::println("[Error] Failed to load glTF");
        abort();
    }

    fmt::println("Loaded {} nodes, {} meshes, {} geometries",
        scene.nodes.size(),
        resourceManager.meshes.size(),
        resourceManager.geometries.size());
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