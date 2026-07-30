#pragma once

#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <vector>
#include <string>
#include <array>
#include <vulkan/vulkan.h>
#include <VkBootstrap.h>
#include <vma/vk_mem_alloc.h>

// windows header contradiction
#ifdef CreateSemaphore
#undef CreateSemaphore
#endif

namespace shzk
{
	class VulkanRHI : public RHI
	{
	public:
		VulkanRHI() = delete;
		VulkanRHI(const RHIInfo& rhiInfo);
		~VulkanRHI() = default;

		virtual void Destroy() override final;
		
	// virtual functions	
		virtual std::shared_ptr<RHIQueue> GetQueue(const RHIQueueInfo& info) override final;
		virtual std::shared_ptr<RHISurface> CreateSurface(SDL_Window* window) override final;
		virtual std::shared_ptr<RHISwapchain> CreateSwapchain(const RHISwapchainInfo& info) override final;
		virtual std::shared_ptr<RHICommandPool> CreateCommandPool(const RHICommandPoolInfo& info) override final;
		virtual std::shared_ptr<RHISemaphore> CreateSemaphore() override final;
		virtual std::shared_ptr<RHIFence> CreateFence() override final;

		inline const VkInstance& GetInstance() { return m_instance; }
		inline const VkPhysicalDevice& GetPhysicalDevice() { return m_physicalDevice; }
		inline const VkDevice& GetDevice() { return m_device; }
		inline const VmaAllocator& GetAllocator() { return m_allocator; }
		

	private:
		void CreateInstance();
		void CreatePhysicalDevice();
		void SelectQueueFamilies();
		void CreateLogicalDevice();
		void CreateMemoryAllocator();
		void CreateQueues();
		void CreateImmediateCommand();
		void CreateDescriptorPool();

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

	class VulkanRHICommandContext : public RHICommandContext
	{
	public:
		VulkanRHICommandContext() = delete;
		VulkanRHICommandContext(VkCommandBuffer vkCmdBuffer, std::shared_ptr<RHICommandPool> cmdPool)
			: m_cmdBuffer(vkCmdBuffer), m_cmdPool(cmdPool) {
		}
		~VulkanRHICommandContext() = default;

		virtual void Destroy() override final;

		inline VkCommandBuffer& GetHandle() { return m_cmdBuffer; }

		// RHI Commands
		virtual void RHIBeginCommand() override final;
		virtual void RHIEndCommand() override final;
		virtual void RHISubmit(
			std::shared_ptr<RHIFence> waitFence,
			std::shared_ptr<RHISemaphore> waitSemaphore,
			std::shared_ptr<RHISemaphore> signalSemaphore) override final;
		virtual void RHITextureClearColor(std::shared_ptr<RHITexture> texture, glm::vec4 rgba) override final;
		virtual void RHITextureBarrierCommand(const RHITextureBarrier& barrier) override final;

	private:
		std::shared_ptr<RHICommandPool> m_cmdPool = nullptr;

		VkCommandBuffer m_cmdBuffer;
	};

	class VulkanRHICommandContextImmediate : public RHICommandContextImmediate
	{
	public:
		VulkanRHICommandContextImmediate() = delete;
		VulkanRHICommandContextImmediate(VulkanRHI& rhi);
		~VulkanRHICommandContextImmediate() = default;

		virtual void Destroy() override final;

	private:
		std::shared_ptr<RHIFence> m_fence;
		std::shared_ptr<RHIQueue> m_queue;
		std::shared_ptr<RHICommandPool> m_cmdPool;

		VkCommandBuffer m_handle;
		VkCommandBuffer m_oldHandle = VK_NULL_HANDLE;
		VkDevice m_device;
	};

	void RHITextureBarrierImpl(VkCommandBuffer& cmdBuffer, const RHITextureBarrier& barrier);
}