#include "VulkanRHISwapchain.h"
#include "VulkanRHI.h"
#include "VulkanRHISurface.h"
#include "VulkanUtil.h"
#include "VulkanRHIResource.h"
#include "VulkanRHIFence.h"
#include "VulkanRHISemaphore.h"
#include "VulkanRHIQueue.h"
#include "runtime/log/Log.h"

#include <iostream>
#include <memory>
#include <algorithm>
#include <volk/volk.h>

namespace shzk
{
	VulkanRHISwapchain::VulkanRHISwapchain(const RHISwapchainInfo& info, VulkanRHI& rhi)
		: RHISwapchain(info)
	{
        VkPhysicalDevice physicalDevice = rhi.GetPhysicalDevice();
        VkDevice device = rhi.GetDevice();
        VkSurfaceKHR surface = std::static_pointer_cast<VulkanRHISurface>(info.surface)->GetHandle();

        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &m_capabilities);

        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        m_availableFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, m_availableFormats.data());

        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &formatCount, nullptr);
        m_availablePresentModes.resize(formatCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &formatCount, m_availablePresentModes.data());

        VkSurfaceFormatKHR surfaceFormat = ChooseSwapchainSurfaceFormat(VulkanUtil::RHIFormatToVkFormat(info.format));
        RHIFormat targetFormat = VulkanUtil::VkFormatToRHIFormat(surfaceFormat.format);

        if (targetFormat != info.format)
        {
            m_info.format = targetFormat;
            SHZK_LOG_ERROR("Cant find swapchain image format support!");
            assert(false);
        }

        VkPresentModeKHR presentMode = ChooseSwapchainPresentMode();

        VkExtent2D extent = ChooseSwapchainExtent();
        if (extent.width != info.extent.width || extent.height != info.extent.height)
        {
            m_info.extent = { extent.width, extent.height };
            SHZK_LOG_ERROR("Cant find suitable swapchain image extent!");
            assert(false);
        }

        uint32_t requestImageCount = std::clamp(info.imageCount, m_capabilities.minImageCount, m_capabilities.maxImageCount);

        VkSwapchainCreateInfoKHR createInfo = {};
        createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
        createInfo.surface = surface;
        createInfo.minImageCount = requestImageCount;
        createInfo.imageFormat = surfaceFormat.format;
        createInfo.imageColorSpace = surfaceFormat.colorSpace;
        createInfo.imageExtent = extent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT |
            VK_IMAGE_USAGE_TRANSFER_SRC_BIT |
            VK_IMAGE_USAGE_TRANSFER_DST_BIT |
            VK_IMAGE_USAGE_SAMPLED_BIT |
            VK_IMAGE_USAGE_STORAGE_BIT;

        createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        createInfo.queueFamilyIndexCount = 0;
        createInfo.pQueueFamilyIndices = nullptr;

        createInfo.preTransform = m_capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = presentMode;
        createInfo.clipped = VK_TRUE;
        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK(vkCreateSwapchainKHR(device, &createInfo, nullptr, &m_handle));

        vkGetSwapchainImagesKHR(device, m_handle, &m_info.imageCount, nullptr);
        m_images.resize(m_info.imageCount);
        vkGetSwapchainImagesKHR(device, m_handle, &m_info.imageCount, m_images.data());

        m_imageFormat = surfaceFormat.format;
        m_imageExtent = extent;
        
        for (uint32_t i = 0; i < m_info.imageCount; i++)
        {
            RHITextureInfo info = {
                .format = targetFormat,
                .extent = { extent.width, extent.height, 1},
                .arrayLayers = 1,
                .mipLevels = 1,
                .memoryUsage = MemoryUsage::GPUOnly,
                .type = RESOURCE_TYPE_TEXTURE | RESOURCE_TYPE_RENDER_TARGET,
                .creationFlag = TEXTURE_CREATION_NONE
            };

            std::shared_ptr<RHITexture> texture = std::make_shared<VulkanRHITexture>(info, rhi, m_images[i]);
            m_textures.push_back(texture);

            // Optional: ResourceState Undefined -> Present
        }
	}

    void VulkanRHISwapchain::Destroy()
    {
        vkDestroySwapchainKHR(VULKAN_RHI()->GetDevice(), m_handle, nullptr);
        SHZK_LOG_INFO("VulkanRHISwapchain destroyed");
    }

    std::shared_ptr<RHITexture> VulkanRHISwapchain::GetTexture(uint32_t index)
    {
        return m_textures[index];
    }

    std::shared_ptr<RHITexture> VulkanRHISwapchain::AcquireNextTexture(std::shared_ptr<RHIFence> fence, std::shared_ptr<RHISemaphore> signalSemaphore)
    {
        VkFence signalFence = VK_NULL_HANDLE;
        VkSemaphore semaphore = VK_NULL_HANDLE;

        if (fence != nullptr) signalFence = CastTo<VulkanRHIFence>(fence)->GetHandle();
        if (signalSemaphore != nullptr) semaphore = CastTo<VulkanRHISemaphore>(signalSemaphore)->GetHandle();

        VK_CHECK(vkAcquireNextImageKHR(
            VULKAN_RHI()->GetDevice(),
            m_handle, UINT64_MAX, semaphore, signalFence, &m_currentIndex));

        return m_textures[m_currentIndex];
    }

    void VulkanRHISwapchain::Present(std::shared_ptr<RHISemaphore> waitSemaphore)
    {
        VkSemaphore semaphore = VK_NULL_HANDLE;
        if (waitSemaphore != nullptr) semaphore = CastTo<VulkanRHISemaphore>(waitSemaphore)->GetHandle();

        VkPresentInfoKHR presentInfo{};
        presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &m_handle;
        presentInfo.pImageIndices = &m_currentIndex;
        presentInfo.pResults = nullptr;
        presentInfo.waitSemaphoreCount = semaphore == VK_NULL_HANDLE ? 0 : 1;
        presentInfo.pWaitSemaphores = &semaphore;

        VK_CHECK(vkQueuePresentKHR(CastTo<VulkanRHIQueue>(m_info.presentQueue)->GetHandle(), &presentInfo));
    }



    VkSurfaceFormatKHR VulkanRHISwapchain::ChooseSwapchainSurfaceFormat(VkFormat targetFormat)
    {
        if (m_availableFormats.size() == 1 && m_availableFormats[0].format == VK_FORMAT_UNDEFINED) {
            return { VK_FORMAT_R8G8B8A8_UNORM, VK_COLOR_SPACE_SRGB_NONLINEAR_KHR };
        }

        for (const auto& format : m_availableFormats)
        {
            if (format.format == targetFormat && format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
            {
                return format;
            }
        }

        return m_availableFormats[0];
    }

    VkPresentModeKHR VulkanRHISwapchain::ChooseSwapchainPresentMode()
    {
        for (const auto& mode : m_availablePresentModes)
        {
            if (mode == VK_PRESENT_MODE_MAILBOX_KHR)
                return mode;
        }
        return VK_PRESENT_MODE_FIFO_KHR;
    }

    VkExtent2D VulkanRHISwapchain::ChooseSwapchainExtent()
    {
        if (m_capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
        {
            return m_capabilities.currentExtent;
        }
        else {
            VkExtent2D actualExtent = { m_info.extent.width, m_info.extent.height };
            actualExtent.width = std::max(m_capabilities.minImageExtent.width, std::min(m_capabilities.maxImageExtent.width, actualExtent.width));
            actualExtent.height = std::max(m_capabilities.minImageExtent.height, std::min(m_capabilities.maxImageExtent.height, actualExtent.height));
            return actualExtent;
        }
    }
}