#include "SwapchainContext.h"

#include <fmt/core.h>

#include "vulkan/VulkanContext.h"

std::unique_ptr<SwapchainContext> CreateSwapchainContext(const VulkanContext& vulkanContext, uint32_t width, uint32_t height)
{
    auto ctx = std::make_unique<SwapchainContext>();

    vk::PhysicalDevice physicalDevice = vulkanContext.physicalDevice;
    vk::Device device = vulkanContext.device;
    vk::SurfaceKHR surface = vulkanContext.surface;

    vk::SurfaceCapabilitiesKHR capabilities =
        physicalDevice.getSurfaceCapabilitiesKHR(surface);

    std::vector<vk::SurfaceFormatKHR> formats =
        physicalDevice.getSurfaceFormatsKHR(surface);

    vk::SurfaceFormatKHR selectedFormat = formats[0];
    for (const auto& fmt : formats)
    {
        if (fmt.format == vk::Format::eB8G8R8A8Srgb &&
            fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
        {
            selectedFormat = fmt;
            break;
        }
    }

    vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
    std::vector<vk::PresentModeKHR> presentModes =
        physicalDevice.getSurfacePresentModesKHR(surface);
    for (const auto& mode : presentModes)
    {
        if (mode == vk::PresentModeKHR::eMailbox)
        {
            presentMode = mode;
            break;
        }
    }

    vk::Extent2D extent;
    if (capabilities.currentExtent.width != UINT32_MAX)
    {
        extent = capabilities.currentExtent;
    }
    else
    {
        extent.width = std::clamp(
            width,
            capabilities.minImageExtent.width,
            capabilities.maxImageExtent.width);
        extent.height = std::clamp(
            height,
            capabilities.minImageExtent.height,
            capabilities.maxImageExtent.height);
    }

    uint32_t imageCount = capabilities.minImageCount + 1;
    if (capabilities.maxImageCount > 0 && imageCount > capabilities.maxImageCount)
        imageCount = capabilities.maxImageCount;

    vk::SwapchainCreateInfoKHR swapchainCI{};
    swapchainCI.setSurface(surface)
        .setMinImageCount(imageCount)
        .setImageFormat(selectedFormat.format)
        .setImageColorSpace(selectedFormat.colorSpace)
        .setImageExtent(extent)
        .setImageArrayLayers(1)
        .setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
        .setPreTransform(capabilities.currentTransform)
        .setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
        .setPresentMode(presentMode)
        .setClipped(true);

    ctx->swapchain = device.createSwapchainKHR(swapchainCI);
    ctx->format = selectedFormat.format;
    ctx->extent = extent;

    auto vkImages = device.getSwapchainImagesKHR(ctx->swapchain);
    ctx->images.resize(vkImages.size());
    for (size_t i = 0; i < vkImages.size(); ++i)
        ctx->images[i] = static_cast<VkImage>(vkImages[i]);

    VkImageViewCreateInfo viewInfo{};
    viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
    viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
    viewInfo.format = static_cast<VkFormat>(ctx->format);
    viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
    viewInfo.subresourceRange.baseMipLevel = 0;
    viewInfo.subresourceRange.levelCount = 1;
    viewInfo.subresourceRange.baseArrayLayer = 0;
    viewInfo.subresourceRange.layerCount = 1;

    ctx->imageViews.resize(ctx->images.size());
    for (size_t i = 0; i < ctx->images.size(); ++i)
    {
        viewInfo.image = ctx->images[i];
        VkImageView v;
        if (vkCreateImageView(
            static_cast<VkDevice>(device),
            &viewInfo, nullptr, &v) != VK_SUCCESS)
        {
            fmt::println("[Vulkan Error] Failed to create swapchain image view {}", i);
            abort();
        }
        ctx->imageViews[i] = v;
    }

    ctx->renderSemaphores.resize(ctx->images.size());
    vk::SemaphoreCreateInfo semInfo{};
    for (size_t i = 0; i < ctx->images.size(); ++i)
    {
        ctx->renderSemaphores[i] = device.createSemaphore(semInfo);
    }

    return ctx;
}