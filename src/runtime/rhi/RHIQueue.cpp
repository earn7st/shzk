#include "RHIQueue.h"

#include "RHICommandList.h"
#include "RHIUtil.h"

namespace vkR
{
	namespace rhi
	{
		void Queue::Initialize(VkDevice device, VkQueue queue, RHIQueueType type, uint32_t index)
		{
			m_device = device;
			m_queue = queue;
			m_type = type;
			m_queueFamilyIndex = index;	

			VkCommandPoolCreateInfo commandPoolCreateInfo;
			commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			commandPoolCreateInfo.pNext = NULL;
			commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
			commandPoolCreateInfo.queueFamilyIndex = index;

			VK_CHECK(vkCreateCommandPool(device, &commandPoolCreateInfo, NULL, &m_cmdPool));

			m_cmdLists.resize(3);
			for (size_t i = 0; i < m_cmdLists.size(); ++i)
			{
				m_cmdLists[i] = std::make_shared<CommandList>();
				m_cmdLists[i]->Initialize(device, this, m_cmdPool, index);
			}
		}

		void Queue::Shutdown()
		{
			for (auto it = m_cmdLists.begin(); it != m_cmdLists.end(); ++it)
			{
				(*it)->Shutdown(m_device);
			}
			m_cmdLists.clear();

			if (m_cmdPool != VK_NULL_HANDLE)
			{
				vkDestroyCommandPool(m_device, m_cmdPool, nullptr);
				m_cmdPool = VK_NULL_HANDLE;
			}

			m_queue = VK_NULL_HANDLE;
			m_device = VK_NULL_HANDLE;
		}

		CommandList* Queue::NextCommandList()
		{
			m_currentCmdListIndex = (m_currentCmdListIndex + 1) % static_cast<uint32_t>(m_cmdLists.size());
			std::shared_ptr<CommandList>& cmdList = m_cmdLists[m_currentCmdListIndex];

			cmdList->WaitForExecution();
			return cmdList.get();
		}

		void Queue::Submit(
			VkCommandBuffer cmd,
			VkPipelineStageFlags waitFlags,
			VkSemaphore semaphoreWait,
			VkSemaphore semaphoreSignal,
			VkFence		fence)
		{
			VkSubmitInfo submitInfo{};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &cmd;

			if (semaphoreWait != VK_NULL_HANDLE)
			{
				submitInfo.waitSemaphoreCount = 1;
				submitInfo.pWaitSemaphores = &semaphoreWait;
				submitInfo.pWaitDstStageMask = &waitFlags;
			}

			if (semaphoreSignal != VK_NULL_HANDLE)
			{
				submitInfo.signalSemaphoreCount = 1;
				submitInfo.pSignalSemaphores = &semaphoreSignal;
			}

			VK_CHECK(vkQueueSubmit(m_queue, 1, &submitInfo, fence));
		}

		bool Queue::Present(VkSwapchainKHR swapchain, uint32_t imageIndex, VkSemaphore renderCompleteSemaphore)
		{

			VkPresentInfoKHR present_info = {};
			present_info.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
			present_info.waitSemaphoreCount = 1;
			present_info.pWaitSemaphores = &renderCompleteSemaphore;
			present_info.swapchainCount = 1;
			present_info.pSwapchains = reinterpret_cast<VkSwapchainKHR*>(&swapchain);
			present_info.pImageIndices = &imageIndex;

			VkResult result = vkQueuePresentKHR(m_queue, &present_info);

			if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
			{
				return false;
			}

			VK_CHECK(result);
			return true;
		}
	}
}