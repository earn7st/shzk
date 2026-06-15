#pragma once

#include "VKCommon.h"

class SDLWindow;

struct VulkanContext
{

public:
	void Init(const SDLWindow& window);

private:
	
	// Vulkan objects
	VkInstance _instance;
	VkPhysicalDevice _gpu;
	VkDevice _device;
	VkSurfaceKHR _surface;
	VkSwapchainKHR _swapchain;
	VkFormat _swapchainImageFormat;
	VkQueue _graphicsQueue;
	uint32_t _graphicsQueueFamily;
	VkDebugUtilsMessengerEXT _debugMessenger;

	// VMA Allocator
	VmaAllocator _allocator;

};