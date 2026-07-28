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
		//SHZK_LOG_INFO("VulkanRHICommandContext destroyed");
	}

	void VulkanRHICommandContext::RHIBeginCommand()
	{
		vkResetCommandBuffer(m_cmdBuffer, 0);

		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		VK_CHECK(vkBeginCommandBuffer(m_cmdBuffer, &beginInfo));
	}

	void VulkanRHICommandContext::RHIEndCommand()
	{
		VK_CHECK(vkEndCommandBuffer(m_cmdBuffer));
	}
}