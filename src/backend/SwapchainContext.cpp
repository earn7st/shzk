#include "SwapchainContext.h"
#include "application/SDLWindow.h"
#include "backend/VulkanContext.h"

#include <VKBootstrap.h>

void SwapchainContext::Initialize(const VulkanContext* vulkanContext, const SDLWindow& window) 
{
    vkb::SwapchainBuilder swapchainBuilder{ vulkanContext->physicalDevice, vulkanContext->device, surface };

    format = vk::Format::eR8G8B8A8Unorm;

    vkb::Swapchain vkbSwapchain = swapchainBuilder
        //.use_default_format_selection()
        .set_desired_format(VkSurfaceFormatKHR{ .format = static_cast<VkFormat>(format), .colorSpace = VK_COLOR_SPACE_SRGB_NONLINEAR_KHR })
        //use vsync present mode
        .set_desired_present_mode(VK_PRESENT_MODE_FIFO_KHR)
        .set_desired_extent(window.extent.width, window.extent.width)
        .add_image_usage_flags(VK_IMAGE_USAGE_TRANSFER_DST_BIT)
        .build()
        .value();

    extent = vkbSwapchain.extent;
    //store swapchain and its related images
    swapchain = vkbSwapchain.swapchain;
    images = vkbSwapchain.get_images().value();
    imageViews = vkbSwapchain.get_image_views().value();
}