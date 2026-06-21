#pragma once

#include <vulkan/vulkan.hpp>

#include <vma/vk_mem_alloc.h>

struct VulkanContext
{
	vk::Instance instance;
	vk::DebugUtilsMessengerEXT debugMessenger;
	vk::Device device;
	vk::PhysicalDevice physicalDevice;
	VmaAllocator allocator;
	uint32_t graphicsQueueFamily;
	vk::Queue graphicsQueue;
	vk::CommandPool commandPool;
};