#include "RHICommandList.h"
#include "RHIUtil.h"
#include "RHIQueue.h"
#include <vulkan/vulkan.h>

namespace vkR
{
	namespace rhi
	{
        void CommandList::Initialize(VkDevice device, Queue* queue, VkCommandPool cmdPool, uint32_t queueFamilyIndex)
        {
            m_device = device;
            m_rhiQueue = queue;
            m_cmdPool = cmdPool;
            
            VkCommandBufferAllocateInfo allocInfo{};
            allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
            allocInfo.commandPool = cmdPool;
            allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
            allocInfo.commandBufferCount = 1;

            VK_CHECK(vkAllocateCommandBuffers(device, &allocInfo, &m_cmd));

            VkFenceCreateInfo fenceInfo{};
            fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;

            VK_CHECK(vkCreateFence(device, &fenceInfo, nullptr, &m_fence));
        }

        void CommandList::Shutdown(VkDevice device)
        {
            if (m_fence != VK_NULL_HANDLE)
            {
                vkDestroyFence(device, m_fence, nullptr);
                m_fence = VK_NULL_HANDLE;
            }

            if (m_cmd != VK_NULL_HANDLE)
            {
                vkFreeCommandBuffers(device, m_cmdPool, 1, &m_cmd);
                m_cmd = VK_NULL_HANDLE;
            }

            m_cmd = VK_NULL_HANDLE;
            m_cmdPool = VK_NULL_HANDLE;
            m_rhiQueue = nullptr;
            m_device = VK_NULL_HANDLE;
        }

        void CommandList::WaitForExecution()
        {
            if (m_state == RHICommandListState::Submitted)
            {
                VK_CHECK(vkWaitForFences(m_device, 1, &m_fence, VK_TRUE, UINT64_MAX));
                VK_CHECK(vkResetFences(m_device, 1, &m_fence));
                m_state = RHICommandListState::Idle;
            }
        }

        void CommandList::Begin()
        {
            if (m_state != RHICommandListState::Idle) {
                printf("[Error] : CommandList::Begin() : m_state not Idle!\n");
                return;
            }

            // begin command buffer
            VkCommandBufferBeginInfo beginInfo = {};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
            beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
            VK_CHECK(vkBeginCommandBuffer(m_cmd, &beginInfo));

            m_state = RHICommandListState::Recording;
        }

        void CommandList::BeginRendering()
        {
            vkCmdBeginRendering(m_cmd, nullptr);
        }

        void CommandList::BeginRendering(
            VkImageView      imageView,
            VkExtent2D       extent,
            VkAttachmentLoadOp loadOp,
            glm::vec4 clearColor)
        {
            VkClearValue clearValue;
            clearValue.color = { clearColor[0], clearColor[1], clearColor[2], clearColor[3] };

            VkRenderingAttachmentInfo colorAttachment{};
            colorAttachment.sType = VK_STRUCTURE_TYPE_RENDERING_ATTACHMENT_INFO;
            colorAttachment.imageView = imageView;
            colorAttachment.imageLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
            colorAttachment.loadOp = loadOp;
            colorAttachment.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
            colorAttachment.clearValue = clearValue;

            VkRenderingInfo renderingInfo{};
            renderingInfo.sType = VK_STRUCTURE_TYPE_RENDERING_INFO;
            renderingInfo.renderArea = { {0, 0}, {extent.width, extent.height} };
            renderingInfo.layerCount = 1;
            renderingInfo.colorAttachmentCount = 1;
            renderingInfo.pColorAttachments = &colorAttachment;

            vkCmdBeginRendering(m_cmd, &renderingInfo);
        }

        void CommandList::EndRendering()
        {
            vkCmdEndRendering(m_cmd);
        }

        void CommandList::Submit(VkSemaphore semaphoreWait, VkSemaphore semaphoreSignal)
        {
            // EndRendering();
            // TODO : Flush Barriers
            VK_CHECK(vkEndCommandBuffer(m_cmd));

            m_rhiQueue->Submit(
                m_cmd,
                VkPipelineStageFlagBits::VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
                semaphoreWait,
                semaphoreSignal,
                m_fence);

            m_state = RHICommandListState::Submitted;
        }

        void CommandList::ImageBarrier(
            VkImage image,
            VkImageLayout oldLayout, VkImageLayout newLayout,
            VkPipelineStageFlags2 srcStage, VkAccessFlags2 srcAccess,
            VkPipelineStageFlags2 dstStage, VkAccessFlags2 dstAccess,
            VkImageAspectFlags aspect,
            uint32_t baseMipLevel, uint32_t levelCount,
            uint32_t baseArrayLayer, uint32_t layerCount)
        {
            VkImageMemoryBarrier2 barrier{};
            barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier.srcStageMask = srcStage;
            barrier.srcAccessMask = srcAccess;
            barrier.dstStageMask = dstStage;
            barrier.dstAccessMask = dstAccess;
            barrier.oldLayout = oldLayout;
            barrier.newLayout = newLayout;
            barrier.image = image;
            barrier.subresourceRange = { aspect, baseMipLevel, levelCount,
                                           baseArrayLayer, layerCount };

            VkDependencyInfo depInfo{};
            depInfo.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            depInfo.imageMemoryBarrierCount = 1;
            depInfo.pImageMemoryBarriers = &barrier;

            vkCmdPipelineBarrier2(m_cmd, &depInfo);
        }
	}
}