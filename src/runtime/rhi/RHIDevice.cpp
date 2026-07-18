#include "RHIDevice.h"
#include "runtime/core/WindowSystem.h"

#include "RHICommandList.h"
#include "RHIQueue.h"
#include "RHIStructs.h"
#include "RHISwapchain.h"
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
        vkb::Device vkbDevice;
    }

    uint32_t Device::kFramesInFlight = 3;

// --- public functions ---

	void Device::Initialize(const RHIInitInfo& initInfo)
	{
		m_window = initInfo.windowSystem->GetWindow();

        CreateInstance();
        CreateSurface();
        CreatePhysicalAndLogicalDevice();
        CreateAllocator();
        CreateQueues();
        CreateSwapchain();
        CreateImmediateCommandPoolGraphics();
        CreateImmediateFence();
        
        /*
        createDescriptorPool();
        */
	}

    void Device::Shutdown()
    {
        vkDeviceWaitIdle(m_device);

        m_graphicsQueue->Shutdown();
        m_computeQueue->Shutdown();
        m_swapchain->Shutdown();

        vkDestroyCommandPool(m_device, m_immediateCommandPool, nullptr);
        vkDestroyFence(m_device, m_immediateFence, nullptr);

        vmaDestroyAllocator(m_allocator);
        vkDestroySurfaceKHR(m_instance, m_surface, nullptr);
        vkDestroyDevice(m_device, nullptr);

        // Destroy both DebugMessenger and Instance by vkb
        vkb::destroy_instance(vkbInstance);
    }

    void Device::ImmediateSubmit(std::function<void(VkCommandBuffer commandBuffer)>&& func)
    {

        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_immediateCommandPool;
        allocInfo.commandBufferCount = 1;

        VkCommandBuffer oneTimeCommandBuffer;
        vkAllocateCommandBuffers(m_device, &allocInfo, &oneTimeCommandBuffer);

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(oneTimeCommandBuffer, &beginInfo);

        func(oneTimeCommandBuffer);

        vkEndCommandBuffer(oneTimeCommandBuffer);

        VkSubmitInfo submitInfo{};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &oneTimeCommandBuffer;

        vkQueueSubmit(m_graphicsQueue->GetQueue(), 1, &submitInfo, m_immediateFence);

        vkWaitForFences(m_device, 1, &m_immediateFence, true, UINT32_MAX);
        vkResetFences(m_device, 1, &m_immediateFence);

        vkFreeCommandBuffers(m_device, m_immediateCommandPool, 1, &oneTimeCommandBuffer);
    }


// --- private functions ---
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

    void Device::CreatePhysicalAndLogicalDevice()
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

        auto deviceRet = deviceBuilder.build();

        if (!deviceRet)
        {
            throw std::runtime_error("[Error] Device::CreateDeviceAndQueues() : Failed to create device");
        }

        vkbDevice = deviceRet.value();

        m_physicalDevice = vkbPhysicalDevice.physical_device;
        m_device = vkbDevice.device;
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

    void Device::CreateQueues()
    {
        m_graphicsQueue = std::make_shared<Queue>();
        m_graphicsQueue->Initialize(
            m_device,
            vkbDevice.get_queue(vkb::QueueType::graphics).value(),
            RHIQueueType::Graphics,
            vkbDevice.get_queue_index(vkb::QueueType::graphics).value());

        m_computeQueue = std::make_shared<Queue>();
        m_computeQueue->Initialize(
            m_device,
            vkbDevice.get_queue(vkb::QueueType::compute).value(),
            RHIQueueType::Compute,
            vkbDevice.get_queue_index(vkb::QueueType::compute).value());
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

    void Device::CreateSwapchain()
    {
        SwapchainSupportDetails swapchainSupport =
            QuerySwapchainSupport(m_physicalDevice, m_surface);

        m_swapchain = std::make_shared<Swapchain>();
        m_swapchain->Initialize(
            m_window,
            m_physicalDevice,
            m_device,
            m_surface,
            swapchainSupport
        );
    }

    void Device::CreateImmediateCommandPoolGraphics()
    {
        VkCommandPoolCreateInfo commandPoolCreateInfo{};
        commandPoolCreateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        commandPoolCreateInfo.pNext = NULL;
        commandPoolCreateInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT | VK_COMMAND_POOL_CREATE_TRANSIENT_BIT;
        commandPoolCreateInfo.queueFamilyIndex = m_graphicsQueue->GetQueueFamilyIndex();
    
        VK_CHECK(vkCreateCommandPool(m_device, &commandPoolCreateInfo, nullptr, &m_immediateCommandPool));
    }

    void Device::CreateImmediateFence()
    {
        VkFenceCreateInfo fenceCreateInfo{};
        fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
        fenceCreateInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

        VK_CHECK(vkCreateFence(m_device, &fenceCreateInfo, nullptr, &m_immediateFence));
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
}