#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHICommandPool : public RHICommandPool
	{
	public:
		VulkanRHICommandPool() = delete;
		VulkanRHICommandPool(const RHICommandPoolInfo& info, VulkanRHI& rhi);
		~VulkanRHICommandPool() = default;

		virtual void Destroy() override final;

		virtual std::shared_ptr<RHICommandContext> CreateCommandContext() override final;

		inline VkCommandPool& GetHandle() { return m_handle; }

	private:
		VkCommandPool m_handle;
		VulkanRHI& m_rhi;
	};
}