#include "application/Application.h"

#include <SDL3/SDL.h>
#include <fmt/core.h>

#include "gltf/gltfLoader.h"

#include "render/descriptor/DescriptorSetLayoutCache.h"

Application::Application(const std::string& name, uint32_t width, uint32_t height) :
	window(name, width, height), renderer(window), sceneRenderer(&scene, &resourceManager)
{
}

void Application::Init()
{
    // Init Default Resoures: fallback Material, ...
    DefaultResourceInit::InitDefaultResources(
        renderer.GetVulkanContext(), 
        renderer.GetUploadContext(), 
        renderer.GetDescriptorAllocator(),
        renderer.GetDescriptorSetLayoutCache()->defaultMaterialLayout,
        & resourceManager);

    gltfLoader loader(
        renderer.GetVulkanContext(),
        renderer.GetUploadContext(),
        & renderer,
        & resourceManager);

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

    // Input
    SDL_SetWindowRelativeMouseMode(window.window, true);

}

void Application::Run()
{
    SDL_Event e;
    bool bWindowShouldClose = false;
    auto lastTime = std::chrono::high_resolution_clock::now();
    while (!bWindowShouldClose)
    {
        while (SDL_PollEvent(&e) != 0) {
            switch (e.type)
            {
            case SDL_EVENT_QUIT:
                bWindowShouldClose = true;
                break;
            
            case SDL_EVENT_WINDOW_RESIZED:
            case SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED:

                break;
            }
        }

        auto now = std::chrono::high_resolution_clock::now();
        deltaTime = std::chrono::duration<double>(now - lastTime).count();
        lastTime = now;
        totalTime += deltaTime;
        
        Tick(deltaTime);

        Render();
    }

}

void Application::Tick(double deltaTime)
{

}

void Application::Render()
{
    if (bSwapchainDirty)
    {
        renderer.ResizeSwapchain(window.extent.width, window.extent.height);
        bSwapchainDirty = false;
    }

    // Swapchain extent should be the only truth aspect source, but here we use window's extent,
    // Only if we carefully keep window's extent and swapchain's extent identical
    float aspect = window.extent.width / window.extent.height;

    FrameUniforms frameData{};
    frameData.view = mainCamera.GetViewMatrix();
    frameData.proj = mainCamera.GetProjectionMatrix(aspect);
    frameData.viewProj = frameData.proj * frameData.view;
    frameData.invViewProj = glm::inverse(frameData.viewProj);
    frameData.cameraPosition = glm::vec4(mainCamera.position, 1.0f);

    frameData.time = { totalTime, deltaTime, std::sin(totalTime), renderer.GetCurrentFrame()};
    frameData.resolution = { window.extent.width, window.extent.height, 1.0f / window.extent.width, 1.0f / window.extent.height };

    frameData.ambientColor = { 0.02f, 0.02f, 0.02f, 1.0f };
    frameData.sunDirection = { 0.5f, 1.0f, 0.3f, 0.0f };
    frameData.sunColor = { 1.0f, 0.95f, 0.8f, 1.0f };

    RenderView view = sceneRenderer.BuildRenderView(frameData);

    renderer.DrawFrame(view);
}

void Application::Shutdown()
{

}

void Application::OnWindwoResized(uint32_t width, uint32_t height)
{
    window.extent.width = width;
    window.extent.height = height;
    bSwapchainDirty = true;
}