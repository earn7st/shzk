#include "VulkanRHICommandContext.h"
#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "VulkanRHICommandPool.h"

#include <memory>
#include <volk/volk.h>

namespace shzk
{

	void VulkanRHICommandContext::Destroy()
	{
		vkFreeCommandBuffers(VULKAN_RHI()->GetDevice(), CastTo<VulkanRHICommandPool>(m_cmdPool)->GetHandle(), 1, &m_cmdBuffer);
		SHZK_LOG_INFO("VulkanRHICommandContext destroyed");
	}

	void VulkanRHICommandContext::BeginCommand()
	{

	}

	void VulkanRHICommandContext::EndCommand()
	{

	}
}