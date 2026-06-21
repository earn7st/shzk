#pragma once

#include <vulkan/vulkan.hpp>

struct SDLWindow;
struct VulkanContext;

struct SwapchainContext
{
	vk::SurfaceKHR surface;
	vk::SwapchainKHR swapchain;
	vk::Format format;
	vk::Extent2D extent;
	std::vector<VkImage> images;
	std::vector<VkImageView> imageViews;

	void Initialize(const VulkanContext* vulkanContext, const SDLWindow& window);
};