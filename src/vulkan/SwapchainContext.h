#pragma once

#include <vector>
#include <vulkan/vulkan.hpp>


struct SDLWindow;
struct VulkanContext;

struct SwapchainContext
{
	vk::SwapchainKHR swapchain;
	vk::Format format;
	vk::Extent2D extent;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;
};

std::unique_ptr<SwapchainContext> CreateSwapchainContext(const VulkanContext& vulkanContext, uint32_t width, uint32_t height);