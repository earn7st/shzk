#include "VulkanRHICommandContextImmediate.h"
#include "VulkanRHI.h"

namespace shzk
{
	VulkanRHICommandContextImmediate::VulkanRHICommandContextImmediate(VulkanRHI& rhi)
	{
		m_fence = rhi.CreateFence();
		m_queue = rhi.GetQueue({.type = RHIQueueType::Graphics, .index = 0});
		m_cmdPool = rhi.CreateCommandPool({ .queue = m_queue });
		m_device = rhi.GetDevice();		// save device handle here
										// because immediate commands can be called frequently, better not be casting global RHI every frame
										
		// VkCommandBuffer is instant created when use
	}
	void VulkanRHICommandContextImmediate::Destroy()
	{
		m_fence->Destroy();
		m_cmdPool->Destroy();
	}
}