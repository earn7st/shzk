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
		VulkanRHICommandContext(VkCommandBuffer vkCmdBuffer, RHICommandPool& cmdPool) 
			: m_cmdBuffer(vkCmdBuffer), m_cmdPool(&cmdPool){}
		~VulkanRHICommandContext() = default;

		virtual void Destroy() override final;

		virtual void BeginCommand() override final;
		virtual void EndCommand() override final;

		inline VkCommandBuffer GetHandle() const { return m_cmdBuffer; }

	private:
		std::shared_ptr<RHICommandPool> m_cmdPool = nullptr;

		VkCommandBuffer m_cmdBuffer;
	};
}