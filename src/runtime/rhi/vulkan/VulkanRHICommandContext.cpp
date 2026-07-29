#include "VulkanRHICommandContext.h"
#include "VulkanRHI.h"
#include "VulkanUtil.h"
#include "VulkanRHICommandPool.h"
#include "VulkanRHIFence.h"
#include "VulkanRHISemaphore.h"
#include "VulkanRHIQueue.h"

#include "runtime/rhi/RHIDefinitions.h"

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

	void VulkanRHICommandContext::RHISubmit(std::shared_ptr<RHIFence> fence, std::shared_ptr<RHISemaphore> waitSemaphore, std::shared_ptr<RHISemaphore> signalSemaphore)
	{
        VkPipelineStageFlags stage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT | VK_PIPELINE_STAGE_TRANSFER_BIT;
        VkFence signalFence = VK_NULL_HANDLE;

        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &m_cmdBuffer;

        if (fence != nullptr)
        {
            signalFence = CastTo<VulkanRHIFence>(fence)->GetHandle();
        }
        if (waitSemaphore != nullptr)
        {
            submitInfo.waitSemaphoreCount = 1;
            submitInfo.pWaitSemaphores = &(CastTo<VulkanRHISemaphore>(waitSemaphore)->GetHandle());
            submitInfo.pWaitDstStageMask = &stage;
        }
        if (signalSemaphore != nullptr)
        {
            submitInfo.signalSemaphoreCount = 1;
            submitInfo.pSignalSemaphores = &(CastTo<VulkanRHISemaphore>(signalSemaphore)->GetHandle());
        }

        VK_CHECK((vkQueueSubmit(CastTo<VulkanRHIQueue>(m_cmdPool->GetQueue())->GetHandle(), 1, &submitInfo, signalFence)));
	}
}