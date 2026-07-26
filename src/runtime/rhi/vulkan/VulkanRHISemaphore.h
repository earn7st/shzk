#pragma once
#include "runtime/rhi/RHI.h"

#include <vulkan/vulkan.h>

namespace shzk
{
	class VulkanRHI;

	class VulkanRHISemaphore : public RHISemaphore
	{
	public:
		VulkanRHISemaphore(VulkanRHI& rhi);
	
	private:
		VkSemaphore m_handle;
	};
}