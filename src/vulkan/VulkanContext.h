#pragma once

#include <vulkan/vulkan.hpp>

#include <vma/vk_mem_alloc.h>

struct SDLWindow;

struct VulkanContext
{
	vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugMessenger;
	vk::SurfaceKHR surface;
	vk::Device device;
	vk::PhysicalDevice physicalDevice;
	uint32_t graphicsQueueFamily;
	vk::Queue graphicsQueue;

	mutable VmaAllocator allocator;
	
};

std::unique_ptr<VulkanContext> CreateVulkanContext(const SDLWindow& window);