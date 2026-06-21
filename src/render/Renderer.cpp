#include "Renderer.h"
#include "application/SDLWindow.h"
#include "backend/VulkanContext.h"
#include "backend/SwapchainContext.h"
#include "scene/Scene.h"

#include <VkBootstrap.h>
#include <SDL3/SDL_vulkan.h>
#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>


Renderer::Renderer() = default;
Renderer::~Renderer() = default;

void Renderer::Initialize(const SDLWindow& window)
{
    //----------------------------------------
    // Bootstrap Phase
    //----------------------------------------
    vkb::InstanceBuilder builder;

    auto instRet = builder
        .set_app_name("vkR")
        .request_validation_layers(true)
        .use_default_debug_messenger()
        .require_api_version(1, 3, 0)
        .build();

    if (!instRet) {
        throw std::runtime_error("Failed to create instance");
    }

    vkb::Instance vkbInstance = instRet.value();

    vk::Instance instance = vkbInstance.instance;
    VkDebugUtilsMessengerEXT debugMessenger =
        vkbInstance.debug_messenger;

    VkSurfaceKHR surface;

    if (!SDL_Vulkan_CreateSurface(
        window.window,
        static_cast<VkInstance>(instance),
        nullptr,
        &surface))
    {
        throw std::runtime_error("Failed to create surface");
    }

    VkPhysicalDeviceVulkan13Features features13{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
        .synchronization2 = true,
        .dynamicRendering = true
    };

    VkPhysicalDeviceVulkan12Features features12{
        .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
        .descriptorIndexing = true,
        .bufferDeviceAddress = true
    };

    vkb::PhysicalDeviceSelector selector{ vkbInstance };

    auto physRet =
        selector
        .set_minimum_version(1, 3)
        .set_required_features_13(features13)
        .set_required_features_12(features12)
        .set_surface(surface)
        .select();

    if (!physRet) {
        throw std::runtime_error("Failed to select GPU");
    }

    vkb::PhysicalDevice vkbPhysicalDevice =
        physRet.value();

    vkb::DeviceBuilder deviceBuilder{
        vkbPhysicalDevice
    };

    auto devRet = deviceBuilder.build();

    if (!devRet) {
        throw std::runtime_error("Failed to create device");
    }

    vkb::Device vkbDevice = devRet.value();

    vk::Device device = vkbDevice.device;

    vk::PhysicalDevice physicalDevice =
        vkbPhysicalDevice.physical_device;

    vk::Queue graphicsQueue =
        vkbDevice.get_queue(vkb::QueueType::graphics).value();

    uint32_t graphicsQueueFamily =
        vkbDevice.get_queue_index(vkb::QueueType::graphics).value();

    VmaAllocator allocator;

    VmaAllocatorCreateInfo allocatorInfo{};
    allocatorInfo.physicalDevice =
        static_cast<VkPhysicalDevice>(physicalDevice);

    allocatorInfo.device =
        static_cast<VkDevice>(device);

    allocatorInfo.instance =
        static_cast<VkInstance>(instance);

    allocatorInfo.flags =
        VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

    vmaCreateAllocator(
        &allocatorInfo,
        &allocator);


    //----------------------------------------
    // Runtime phase
    //----------------------------------------

    // Vulkan Context
    vulkanContext = std::make_unique<VulkanContext>();

    vulkanContext->instance = instance;
    vulkanContext->debugMessenger = debugMessenger;

    vulkanContext->physicalDevice = physicalDevice;
    vulkanContext->device = device;

    vulkanContext->graphicsQueue = graphicsQueue;
    vulkanContext->graphicsQueueFamily =
        graphicsQueueFamily;

    vulkanContext->allocator = allocator;

    // Swapchain Context
    swapchainContext = std::make_unique<SwapchainContext>();

    swapchainContext->surface = surface;

    swapchainContext->Initialize(
        vulkanContext.get(),
        window);

}

