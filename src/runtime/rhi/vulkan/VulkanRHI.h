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
		virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) override final;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) override final;
		//virtual std::shared_ptr<RHISwapchain> CreateSwapchain() override final;

	// Getters (for Debug, Vulkan Handles)
		inline VkInstance GetInstance() const { return m_instance; }
		inline VkDevice GetDevice() const { return m_device; }
		inline VmaAllocator GetAllocator() const { return m_allocator; }

	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void SelectQueueFamilies();
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
		std::array<int32_t, (size_t)RHIQueueType::Max> m_queueIndices;	// Family indices for a specific queue type
		std::array<int32_t, (size_t)RHIQueueType::Max> m_queueCounts;	// How many queues we have for a specific queue type
		std::array<std::array<std::shared_ptr<RHIQueue>, MAX_QUEUE_CNT>, (size_t)RHIQueueType::Max> m_queues;	// Containing RHIQueue type
	};

// RHIQueue
	class VulkanRHIQueue : public RHIQueue
	{
	public:
		VulkanRHIQueue(const RHIQueueInfo& info, VkQueue queue, uint32_t queueFamilyIndex)
			: RHIQueue(info), m_handle(queue), m_queueFamilyIndex(queueFamilyIndex)
		{
		}

		// Getters
		inline VkQueue GetHandle() const { return m_handle; }
		inline uint32_t GetQueueFamilyIndex() const { return m_queueFamilyIndex; }

	private:
		VkQueue m_handle;
		uint32_t m_queueFamilyIndex;
	};

// RHISurface
	class VulkanRHISurface : public RHISurface
	{
	public:
		VulkanRHISurface() = delete;
		VulkanRHISurface(SDL_Window* window, VulkanRHI& rhi);

		inline VkSurfaceKHR GetHandle() const { return m_handle; }

	private:
		VkSurfaceKHR m_handle;
	};
}