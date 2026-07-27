#include "VulkanRHISemaphore.h"
#include "VulkanRHI.h"
#include "VulkanUtil.h"

#include <volk/volk.h>

namespace shzk
{
	VulkanRHISemaphore::VulkanRHISemaphore(VulkanRHI& rhi)
	{
		VkSemaphoreCreateInfo semaphoreCreateInfo{};
		semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		VK_CHECK(vkCreateSemaphore(rhi.GetDevice(), &semaphoreCreateInfo, nullptr, &m_handle));
	}

	void VulkanRHISemaphore::Destroy()
	{
		vkDestroySemaphore(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
		SHZK_LOG_INFO("VulkanRHISemaphore destroyed");
	}
}