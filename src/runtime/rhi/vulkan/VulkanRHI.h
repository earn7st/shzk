#pragma once

#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vector>
#include <string>
#include <array>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

class SDL_Window;

namespace shzk
{
	class VulkanRHI : public RHI
	{
	public:
		VulkanRHI() = delete;
		VulkanRHI(const RHIInfo& rhiInfo);
		~VulkanRHI() = default;


	// virtual functions	
		//virtual std::shared_ptr<RHIQueue> GetQueue() override final;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) override final;
		//virtual std::shared_ptr<RHISwapchain> CreateSwapchain() override final;

	// Getters (Vulkan)
		inline VkInstance GetInstance() const { return m_instance; }
		inline VkDevice GetDevice() const { return m_device; }
		inline VmaAllocator GetAllocator() const { return m_allocator; }

	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void CreateLogicalDevice();
		void CreateMemoryAllocator();
		void CreateQueues();
		void CreateDescriptorPool();
		void CreateImmediateCommand();

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

		std::vector<VkQueueFamilyProperties> m_queueFamilyProperties;
		std::array<int32_t, (size_t)RHIQueueType::Max> m_queueIndices;
		std::array<std::array<std::shared_ptr<RHIQueue>, MAX_QUEUE_CNT>, (size_t)RHIQueueType::Max> m_queues;
	};
}