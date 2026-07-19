#pragma once

#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace vkR
{
	namespace rhi
	{
		class Queue;

		enum class RHICommandListState : uint8_t
		{
			Idle,
			Recording,
			Submitted
		};

		class CommandList
		{
		public:
			CommandList() = default;
			~CommandList() = default;

			void Initialize(VkDevice device, Queue* queue, VkCommandPool cmdPool, uint32_t queueFamilyIndex);
			void Shutdown(VkDevice device);

			void WaitForExecution();
			void Begin();

			void BeginRendering();
			void BeginRendering(
				VkImageView      imageView,
				VkExtent2D       extent,
				VkAttachmentLoadOp loadOp,
				glm::vec4 clearColor = { 0.7f, 0.0f, 0.7f, 1.0f });	// FOR NOW, To be encapsulated
			
			void EndRendering();			
			
			void Submit(VkSemaphore semaphoreWait, VkSemaphore semaphore_signal);

			void ImageBarrier(
				VkImage image,
				VkImageLayout oldLayout, VkImageLayout newLayout,
				VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,
				VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess,
				VkImageAspectFlags aspect,
				uint32_t baseMipLevel = 0, uint32_t levelCount = 1,
				uint32_t baseArrayLayer = 0, uint32_t layerCount = 1);

			const RHICommandListState GetState() const { return m_state; }
			Queue* GetQueue() const { return m_rhiQueue; }

		private:
			Queue* m_rhiQueue = nullptr;

			VkDevice m_device = VK_NULL_HANDLE;
			VkCommandPool m_cmdPool = VK_NULL_HANDLE;
			VkCommandBuffer m_cmd = VK_NULL_HANDLE;
			VkFence m_fence = VK_NULL_HANDLE;
			
			RHICommandListState m_state = RHICommandListState::Idle;
		};
	}
}