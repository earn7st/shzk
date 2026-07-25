#pragma once

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHICommandContext £º public RHICommandContext
	{
	public:

	private:
		VkCommandBuffer m_buffer;
	};
}