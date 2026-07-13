#include "RHIDevice.h"
#include "runtime/core/WindowSystem.h"

#include "RHIStructs.h"

#include "RHIUtil.h"

#include <algorithm>
#include <VkBootstrap.h>
#include <SDL3/SDL_vulkan.h>

#define VMA_IMPLEMENTATION
#include <vma/vk_mem_alloc.h>

namespace vkR::rhi
{
    namespace 
    {
        vkb::Instance vkbInstance;
    }

    uint32_t Device::kFramesInFlight = 3;

	void Device::Initialize(const RHIInitInfo& initInfo)
	{
		m_window = initInfo.windowSystem->GetWindow();

        CreateInstance();
        CreateSurface();
        CreateDeviceAndQueues();
        CreateAllocator();
        
        CreateImmediateCommandPool();
        CreateFrameCommandPools();
        CreateFrameCommandBuffers();
        CreateFrameSyncs();

        CreateSwapchain();
        CreateImageViews();

        
        /*
        createDescriptorPool();
        */
	}

    void Device::CreateInstance()
    {
        vkb::InstanceBuilder builder;

        auto instanceRet = builder
            .set_app_name("vkR")
            .request_validation_layers(true)
            .use_default_debug_messenger()
            .require_api_version(1, 3, 0)
            .build();

        if (!instanceRet) {
            throw std::runtime_error("[Error] Device::CretaeInstance() : Failed to create instance");
        }

        vkbInstance = instanceRet.value();
        m_instance = vkbInstance.instance;
        
        m_debugMessenger = vkbInstance.debug_messenger;
    }

    void Device::CreateSurface()
    {

        if (!SDL_Vulkan_CreateSurface(
            m_window,
            m_instance,
            nullptr,
            &m_surface))
        {
            throw std::runtime_error("[Error] Device::CreateSurface() : Failed to create surface");
        }
    }

    void Device::CreateDeviceAndQueues()
    {
        VkPhysicalDeviceVulkan13Features features13{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_3_FEATURES,
            .synchronization2 = true,
            .dynamicRendering = true
        };

        VkPhysicalDeviceVulkan12Features features12{
            .sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_2_FEATURES,
            .descriptorIndexing = true,
            .bufferDeviceAddress = true
        };

        vkb::PhysicalDeviceSelector selector{ vkbInstance };

        auto physRet = selector
            .set_minimum_version(1, 3)
            .set_required_features_13(features13)
            .set_required_features_12(features12)
            .set_surface(m_surface)
            .select();

        if (!physRet) {
            throw std::runtime_error("[Error] Device::CreateDeviceAndQueues() : Failed to select GPU");
        }

        vkb::PhysicalDevice vkbPhysicalDevice = physRet.value();

        vkb::DeviceBuilder deviceBuilder{ vkbPhysicalDevice };

        auto devRet = deviceBuilder.build();

        if (!devRet)
        {
            throw std::runtime_error("[Error] Device::CreateDeviceAndQueues() : Failed to create device");
        }

        vkb::Device vkbDevice = devRet.value();

        m_physicalDevice = vkbPhysicalDevice.physical_device;
        m_device = vkbDevice.device;

        m_queueFamilyIndices.graphicsFamily = vkbDevice.get_queue_index(vkb::QueueType::graphics).value();
		m_queueFamilyIndices.presentFamily = vkbDevice.get_queue_index(vkb::QueueType::present).value();
		m_queueFamilyIndices.computeFamily = vkbDevice.get_queue_index(vkb::QueueType::compute).value();

        m_graphicsQueue = vkbDevice.get_queue(vkb::QueueType::graphics).value();
    }

    void Device::CreateAllocator()
    {
        VmaAllocatorCreateInfo allocatorInfo{};
        allocatorInfo.physicalDevice = m_physicalDevice;
        allocatorInfo.device = m_device;
        allocatorInfo.instance = m_instance;
        allocatorInfo.flags = VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

        vmaCreateAllocator(&allocatorInfo, &m_allocator);
    }

    void Device::CreateSwapchain()
    {
		SwapchainSupportDetails swapchainSupport = std::move(QuerySwapchainSupport(m_physicalDevice, m_surface));
        
        VkSurfaceFormatKHR chosenSurfaceFormat =
            ChooseSwapchainSurfaceFormatFromDetails(swapchainSupport.formats);

        VkPresentModeKHR chosenPresentMode =
            ChooseSwapchainPresentModeFromDetails(swapchainSupport.presentModes);

        VkExtent2D chosenExtent = ChooseSwapchainExtentFromDetails(swapchainSupport.capabilities);

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
        createInfo.imageFormat = chosenSurfaceFormat.format;
        createInfo.imageColorSpace = chosenSurfaceFormat.colorSpace;
        createInfo.imageExtent = chosenExtent;
        createInfo.imageArrayLayers = 1;
        createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

        uint32_t queueFamilyIndices[] = { m_queueFamilyIndices.graphicsFamily.value(), m_queueFamilyIndices.presentFamily.value() };

        if (m_queueFamilyIndices.graphicsFamily != m_queueFamilyIndices.presentFamily)
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
            createInfo.queueFamilyIndexCount = 2;
            createInfo.pQueueFamilyIndices = queueFamilyIndices;
        }
        else
        {
            createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
        }

        createInfo.preTransform = swapchainSupport.capabilities.currentTransform;
        createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
        createInfo.presentMode = chosenPresentMode;
        createInfo.clipped = VK_TRUE;

        createInfo.oldSwapchain = VK_NULL_HANDLE;

        VK_CHECK(vkCreateSwapchainKHR(m_device, &createInfo, nullptr, &m_swapchain));

        VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, nullptr));
        m_swapchainImages.resize(imageCount);
        VK_CHECK(vkGetSwapchainImagesKHR(m_device, m_swapchain, &imageCount, m_swapchainImages.data()));

		// Make kFramesInFlight equals to the number of swapchain images
		kFramesInFlight = imageCount;

        m_swapchainFormat = chosenSurfaceFormat.format;
        m_swapchainExtent.height = chosenExtent.height;
        m_swapchainExtent.width = chosenExtent.width;

        m_scissor = { {0, 0}, {m_swapchainExtent.width, m_swapchainExtent.height} };
    }

    void Device::CreateImageViews()
    {
        m_swapchainImageViews.resize(m_swapchainImages.size());
        
        for (size_t i = 0; i < m_swapchainImages.size(); i++)
        {
            m_swapchainImageViews[i] = RHIUtil::CreateImageView(
                m_device,
                m_swapchainImages[i],
                m_swapchainFormat,
                VK_IMAGE_ASPECT_COLOR_BIT,
                VK_IMAGE_VIEW_TYPE_2D,
                1,
                1);
        }
    }

    void Device::CreateImmediateCommandPool()
    {

        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = NULL;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
        commandPoolCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();

        VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_immediateCommandPool));
    }

    void Device::CreateFrameCommandPools()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo;
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = NULL;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        commandPoolCreateInfo.queueFamilyIndex = m_queueFamilyIndices.graphicsFamily.value();

        m_commandPools.resize(kFramesInFlight);

        for (uint32_t i = 0; i < kFramesInFlight; ++i)
        {
            VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, NULL, &m_commandPools[i]));
        }
    }

    void Device::CreateFrameCommandBuffers()
    {
        VkCommandBufferAllocateInfo commandBufferAllocateInfo{};
        commandBufferAllocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        commandBufferAllocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        commandBufferAllocateInfo.commandBufferCount = 1U;

        m_commandBuffers.resize(kFramesInFlight);

        for (uint32_t i = 0; i < kFramesInFlight; ++i)
        {
            commandBufferAllocateInfo.commandPool = m_commandPools[i];
            VK_CHECK(vkAllocateCommandBuffers(m_device, &commandBufferAllocateInfo, &m_commandBuffers[i]));
        }
    }

    void Device::CreateFrameSyncs()
    {
        VkSemaphoreCreateInfo semaphoreCreateInfo{};
        semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT; // the fence is initialized as signaled

        m_imageAcquiredSemaphores.resize(kFramesInFlight);
        m_renderFinishedSemaphore.resize(kFramesInFlight);
        m_fences.resize(kFramesInFlight);

        for (uint32_t i = 0; i < kFramesInFlight; i++)
        {
            VK_CHECK(vkCreateSemaphore(
                m_device,
                &semaphoreCreateInfo,
                nullptr,
                &m_imageAcquiredSemaphores[i]));

            VK_CHECK(vkCreateSemaphore(
                m_device,
                &semaphoreCreateInfo,
                nullptr,
                &m_renderFinishedSemaphore[i]));

            VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_fences[i]));
        }
    }

    SwapchainSupportDetails Device::QuerySwapchainSupport(VkPhysicalDevice physicalDevice, VkSurfaceKHR surface)
    {
        SwapchainSupportDetails details;
        vkGetPhysicalDeviceSurfaceCapabilitiesKHR(physicalDevice, surface, &details.capabilities);
        uint32_t formatCount;
        vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, nullptr);
        if (formatCount != 0)
        {
            details.formats.resize(formatCount);
            vkGetPhysicalDeviceSurfaceFormatsKHR(physicalDevice, surface, &formatCount, details.formats.data());
        }
        uint32_t presentModeCount;
        vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, nullptr);
        if (presentModeCount != 0)
        {
            details.presentModes.resize(presentModeCount);
            vkGetPhysicalDeviceSurfacePresentModesKHR(physicalDevice, surface, &presentModeCount, details.presentModes.data());
        }
        return details;
    }

    VkSurfaceFormatKHR Device::ChooseSwapchainSurfaceFormatFromDetails(const std::vector<VkSurfaceFormatKHR>& formats)
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
        
    VkPresentModeKHR Device::ChooseSwapchainPresentModeFromDetails(const std::vector<VkPresentModeKHR>& availablePresentModes)
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

    VkExtent2D Device::ChooseSwapchainExtentFromDetails(const VkSurfaceCapabilitiesKHR& capabilities)
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