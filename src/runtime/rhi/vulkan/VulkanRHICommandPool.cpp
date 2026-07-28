#include "VulkanRHICommandPool.h"
#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "VulkanRHICommandContext.h"

#include <volk/volk.h>

namespace shzk
{
	VulkanRHICommandPool::VulkanRHICommandPool(const RHICommandPoolInfo& info, VulkanRHI& rhi)
		: RHICommandPool(info), m_rhi(rhi)
	{
		VkCommandPoolCreateInfo cmdPoolCreateInfo{};
		cmdPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		cmdPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
		cmdPoolCreateInfo.queueFamilyIndex = info.queue->GetQueueFamilyIndex();
		VK_CHECK(vkCreateCommandPool(rhi.GetDevice(), &cmdPoolCreateInfo, nullptr, &m_handle));
	}

	void VulkanRHICommandPool::Destroy()
	{
		vkDestroyCommandPool(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
		//SHZK_LOG_INFO("VulkanRHICommandPool destroyed");
	}

	std::shared_ptr<RHICommandContext> VulkanRHICommandPool::CreateCommandContext()
	{
		VkCommandBuffer cmdBuffer;
		VkCommandBufferAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = m_handle;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		// Command Pool 为了方便创建 Command Context 保存了 VulkanRHI: m_rhi
		// 不然的话每次 CreateCommandContext 还需要 RHI cast 到 vulkanRHI
		VK_CHECK(vkAllocateCommandBuffers(m_rhi.GetDevice(), &allocInfo, &cmdBuffer));

		std::shared_ptr<RHICommandContext> cmdContext = std::make_shared<VulkanRHICommandContext>(cmdBuffer, shared_from_this());
		assert(cmdContext);
		SHZK_LOG_INFO("RHICommandContext created");
		return cmdContext;
	}
}