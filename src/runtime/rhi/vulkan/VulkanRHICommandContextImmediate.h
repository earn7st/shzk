#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

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
}