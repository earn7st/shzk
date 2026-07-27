#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHIFence : public RHIFence
	{
	public:
		VulkanRHIFence() = delete;
		VulkanRHIFence(VulkanRHI& rhi);
		~VulkanRHIFence() = default;
		
		virtual void Destroy() override final;

		virtual void Wait() override final;

	private:
		VkFence m_handle;
	};
}