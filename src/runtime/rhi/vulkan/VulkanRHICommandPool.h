#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHICommandPool : public RHICommandPool
	{
	public:
		VulkanRHICommandPool(const RHICommandPoolInfo& info, VulkanRHI& rhi);

		virtual std::shared_ptr<RHICommandContext> CreateCommandContext() override final;

		inline VkCommandPool GetHandle() const { return m_handle; }

	private:
		VkCommandPool m_handle;
		VulkanRHI& m_rhi;
	};
}