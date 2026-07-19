#pragma once

#include <memory>
#include <vector>
#include <vulkan/vulkan.h>

namespace vkR
{
	namespace rhi
	{
		class CommandList;

		enum class RHIQueueType
		{
			Graphics,
			Compute,
			Copy,
			Max
		};

		class Queue
		{
		public:
			Queue()		= default;
			~Queue()	= default;

			void Initialize(VkDevice device, VkQueue queue, RHIQueueType type, uint32_t index);
			void Shutdown();

			CommandList* NextCommandList();

			void Submit(
				VkCommandBuffer cmd, 
				VkPipelineStageFlags waitFlags,
				VkSemaphore semaphoreWait, 
				VkSemaphore semaphoreSignal,
				VkFence	fence);
			bool Present(VkSwapchainKHR swapchain, uint32_t imageIndex, VkSemaphore renderCompleteSemaphore);

			// --- Getters ---
			VkQueue GetQueue() const { return m_queue; }
 			uint32_t GetQueueFamilyIndex() const { return m_queueFamilyIndex; }

		private:
			VkDevice m_device = VK_NULL_HANDLE;

			VkQueue m_queue = VK_NULL_HANDLE;
			RHIQueueType m_type = RHIQueueType::Max;
			uint32_t m_queueFamilyIndex = 0;

			VkCommandPool m_cmdPool = VK_NULL_HANDLE;
			std::vector<std::shared_ptr<CommandList>> m_cmdLists;
			uint32_t m_currentCmdListIndex = 0;

		};
	}
}