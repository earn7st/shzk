#include "VulkanRHIFence.h"
#include "VulkanUtil.h"
#include "VulkanRHI.h"

#include <volk/volk.h>

namespace shzk
{
	VulkanRHIFence::VulkanRHIFence(VulkanRHI& rhi)
	{
		VkFenceCreateInfo fenceCreateInfo{};
		fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;	// fence created as SIGNALED as default
		VK_CHECK(vkCreateFence(rhi.GetDevice(), &fenceCreateInfo, nullptr, &m_handle));
	}

	void VulkanRHIFence::Destroy()
	{
		vkDestroyFence(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
		SHZK_LOG_INFO("VulkanRHIFence destroyed");
	}

	void VulkanRHIFence::Wait()
	{
		// TODO
	}

}