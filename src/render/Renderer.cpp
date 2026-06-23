#include "Renderer.h"
#include <VkBootstrap.h>
#include <SDL3/SDL_vulkan.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>
#include "application/SDLWindow.h"
#include "scene/Scene.h"
#include "vulkan/VulkanContext.h"
#include "vulkan/SwapchainContext.h"
#include "vulkan/UploadContext.h"



Renderer::Renderer() = default;

Renderer::Renderer(const SDLWindow& window) 
{
    // VulkanContext
    vulkanContext = CreateVulkanContext(window);    

    // Swapchain Context
    swapchainContext = CreateSwapchainContext(*vulkanContext, window.extent.width, window.extent.height);
    
    // Upload Context (for Immediate Submit)
    uploadContext = CreateUploadContext(*vulkanContext);
}

Renderer::~Renderer() = default;

