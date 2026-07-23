#pragma once

#include "runtime/rhi/RHI.h"

#include <vector>
#include <string>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

namespace shzk
{
	class VulkanRHI : public RHI
	{
	public:
		VulkanRHI() = delete;
		VulkanRHI(const RHIInfo& rhiInfo);
		~VulkanRHI() = default;

	private:

		vkb::Instance m_vkbInstance;
		vkb::PhysicalDevice m_vkbPhysicalDevice;

		VkInstance m_instance;
		VkDebugUtilsMessengerEXT m_debugMessenger;

		VkPhysicalDevice m_physicalDevice;
		VkPhysicalDeviceProperties m_properties;
		VkPhysicalDeviceFeatures m_features;
		VkPhysicalDeviceMemoryProperties m_memoryProperties;
		std::vector<std::string> m_supportedExtensions;

		VkDevice m_device;

		VmaAllocator m_allocator;
	
	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void CreateLogicalDevice();
		void CreateMemoryAllocator();
		void CreateQueues();
		void CreateDescriptorPool();
		void CreateImmediateCommand();
	};
}