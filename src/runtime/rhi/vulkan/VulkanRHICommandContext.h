#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHICommandPool;

	class VulkanRHICommandContext : public RHICommandContext
	{
	public:
		VulkanRHICommandContext() = delete;
		VulkanRHICommandContext(VkCommandBuffer vkCmdBuffer, std::shared_ptr<RHICommandPool> cmdPool)
			: m_cmdBuffer(vkCmdBuffer), m_cmdPool(cmdPool){}
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

	private:
		std::shared_ptr<RHICommandPool> m_cmdPool = nullptr;

		VkCommandBuffer m_cmdBuffer;
	};
}