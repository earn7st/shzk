#pragma once

#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHIFence : public RHIFence
	{
	public:
		VulkanRHIFence(VulkanRHI& rhi);

		virtual void Wait() override final;

	private:
		VkFence m_handle;
	};
}