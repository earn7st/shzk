#include "RHISwapchain.h"
#include "RHICommandList.h"
#include "RHIQueue.h"
#include "RHIDefs.h"
#include "RHIUtil.h"

#include <algorithm>
#include <vulkan/vulkan.h>
#include <SDL3/SDL.h>

namespace vkR
{
	namespace rhi
	{
// --- public functions ---
        void Swapchain::Initialize(
            SDL_Window* window,
            VkPhysicalDevice physicalDevice,
            VkDevice         device,
            VkSurfaceKHR     surface,
            const SwapchainSupportDetails& swapchainSupport)
        {
            m_window = window;
            m_device = device;
            m_surface = surface;

            VkSurfaceFormatKHR format = ChooseSwapchainSurfaceFormatFromDetails(swapchainSupport.formats);
            VkPresentModeKHR   mode = ChooseSwapchainPresentModeFromDetails(swapchainSupport.presentModes);
            VkExtent2D         extent = ChooseSwapchainExtentFromDetails(swapchainSupport.capabilities);

            m_format = format.format;
            m_extent = extent;

            uint32_t imageCount = swapchainSupport.capabilities.minImageCount + 1;
            if (swapchainSupport.capabilities.maxImageCount > 0 &&
                imageCount > swapchainSupport.capabilities.maxImageCount)
            {
                imageCount = swapchainSupport.capabilities.maxImageCount;
            }

            VkSwapchainCreateInfoKHR createInfo{};
            createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
            createInfo.surface = m_surface;
            createInfo.minImageCount = imageCount;
            createInfo.imageFormat = m_format;
            createInfo.imageColorSpace = format.colorSpace;
            createInfo.imageExtent = m_extent;
            createInfo.imageArrayLayers = 1;
            createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
            createInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
            createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
            createInfo.presentMode = mode;
            createInfo.clipped = VK_TRUE;
            createInfo.oldSwapchain = VK_NULL_HANDLE;

            VK_CHECK(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain));

            uint32_t actualCount = 0;
            vkGetSwapchainImagesKHR(m_device, m_swapchain, &actualCount, nullptr);
            m_images.resize(actualCount);
            vkGetSwapchainImagesKHR(m_device, m_swapchain, &actualCount, m_images.data());

            m_imageViews.resize(actualCount);
            for (uint32_t i = 0; i < actualCount; i++)
            {
                VkImageViewCreateInfo viewInfo{};
                viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
                viewInfo.image = m_images[i];
                viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
                viewInfo.format = m_format;
                viewInfo.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
                viewInfo.subresourceRange.levelCount = 1;
                viewInfo.subresourceRange.layerCount = 1;

                VK_CHECK(vkCreateImageView(m_device, &viewInfo, nullptr, &m_imageViews[i]));
            }

            m_imageAcquiredSemaphores.resize(actualCount);
            m_renderCompleteSemaphores.resize(actualCount);
            VkSemaphoreCreateInfo semaphoreInfo{};
            semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
            for (uint32_t i = 0; i < actualCount; i++)
            {
                VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
                    &m_imageAcquiredSemaphores[i]));
                VK_CHECK(vkCreateSemaphore(m_device, &semaphoreInfo, nullptr,
                    &m_renderCompleteSemaphores[i]));
            }
        }

        void Swapchain::Shutdown()
        {
            for (auto& view : m_imageViews)
            {
                if (view != VK_NULL_HANDLE)
                {
                    vkDestroyImageView(m_device, view, nullptr);
                }
            }
            m_imageViews.clear();
            m_images.clear();

            for (auto& sem : m_imageAcquiredSemaphores)
            {
                if (sem != VK_NULL_HANDLE)
                {
                    vkDestroySemaphore(m_device, sem, nullptr);
                }
            }
            m_imageAcquiredSemaphores.clear();

            for (auto& sem : m_renderCompleteSemaphores)
            {
                if (sem != VK_NULL_HANDLE)
                {
                    vkDestroySemaphore(m_device, sem, nullptr);
                }
            }
            m_renderCompleteSemaphores.clear();

            if (m_swapchain != VK_NULL_HANDLE)
            {
                vkDestroySwapchainKHR(m_device, m_swapchain, nullptr);
                m_swapchain = VK_NULL_HANDLE;
            }

            m_device = VK_NULL_HANDLE;
            m_surface = VK_NULL_HANDLE;
        }

        void Swapchain::AcquireNextImage()
        {
            m_imageAcquired = false;

            VkResult result = vkAcquireNextImageKHR(
                m_device, m_swapchain, UINT64_MAX,
                m_imageAcquiredSemaphores[m_semaphoreIndex], 
                VK_NULL_HANDLE,
                &m_acquiredImageIndex);

            if (result == VK_SUCCESS || result == VK_SUBOPTIMAL_KHR)
            {
                std::swap(m_imageAcquiredSemaphores[m_acquiredImageIndex], m_imageAcquiredSemaphores[m_semaphoreIndex]);
                m_semaphoreIndex = (m_semaphoreIndex + 1) % m_imageAcquiredSemaphores.size();
                m_imageAcquired = true;
                return;
            }
        }

        void Swapchain::Present(CommandList* cmdListPresent)
        {
            if (!m_imageAcquired) return;

            VkSemaphore renderCompleteSemaphore = m_renderCompleteSemaphores[m_acquiredImageIndex];
            bool success = cmdListPresent->GetQueue()->Present(m_swapchain, m_acquiredImageIndex, renderCompleteSemaphore);

            m_imageAcquired = false;

            // Needs recreation
            if (!success) m_isDirty = true;

            if (m_isDirty)
            {
                // TODO: Recreate
                //Create();
                m_isDirty = false;
            }
        }

// --- private functions ---
        VkSurfaceFormatKHR Swapchain::ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& formats)
        {
            for (const auto& format : formats)
            {
                if (format.format == VK_FORMAT_B8G8R8A8_UNORM &&
                    format.colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR)
                {
                    return format;
                }
            }
            return formats[0];
        }

        VkPresentModeKHR Swapchain::ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes)
        {
            for (VkPresentModeKHR presentMode : availablePresentModes)
            {
                if (VK_PRESENT_MODE_MAILBOX_KHR == presentMode)
                {
                    return VK_PRESENT_MODE_MAILBOX_KHR;
                }
            }
            return VK_PRESENT_MODE_FIFO_KHR;
        }

        VkExtent2D Swapchain::ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities)
        {
            if (capabilities.currentExtent.width != UINT32_MAX)
            {
                return capabilities.currentExtent;
            }
            else
            {
                int width, height;
                SDL_GetWindowSize(m_window, &width, &height);

                VkExtent2D actualExtent = { static_cast<uint32_t>(width), static_cast<uint32_t>(height) };

                actualExtent.width =
                    std::clamp(actualExtent.width, capabilities.minImageExtent.width, capabilities.maxImageExtent.width);
                actualExtent.height =
                    std::clamp(actualExtent.height, capabilities.minImageExtent.height, capabilities.maxImageExtent.height);

                return actualExtent;
            }
        }
	}
}