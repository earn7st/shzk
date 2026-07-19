#include "RHIDevice.h"
#include "runtime/core/WindowSystem.h"

#include "RHIBuffer.h"
#include "RHICommandList.h"
#include "RHIDefs.h"
#include "RHIQueue.h"
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

        CreateExtFunctions();
        
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

    bool Device::MemoryCreateBuffer(VkBuffer* outBuffer, VmaAllocation* outAllocation, void** outMappedData, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags memoryFlags, const char* name)
    {
        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};

        bool isHostVisible = (memoryFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0;
        bool isDeviceLocal = (memoryFlags & VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT) != 0;

        if (isDeviceLocal && !isHostVisible)
        {
            allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
        }
        else if (isHostVisible)
        {
            allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
            allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
                | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;
        }

        VmaAllocationInfo allocInfo2{};
        VkResult result = vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo,
            outBuffer, outAllocation, &allocInfo2);

        if (result != VK_SUCCESS)
        {
            fmt::println("[Device] Failed to create buffer '{}'", name);
            return false;
        }

        *outMappedData = isHostVisible ? allocInfo2.pMappedData : nullptr;

        if (m_pfnSetDebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT nameInfo{};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
            nameInfo.objectHandle = reinterpret_cast<uint64_t>(*outBuffer);
            nameInfo.pObjectName = name;
            m_pfnSetDebugUtilsObjectNameEXT(m_device, &nameInfo);
        }
        return true;
    }

    void Device::MemoryDestroyBuffer(VkBuffer buffer, VmaAllocation allocation)
    {
        if (buffer != VK_NULL_HANDLE)
        {
            vmaDestroyBuffer(m_allocator, buffer, allocation);
        }
    }

    void Device::UploadDataToBuffer(VkBuffer dstBuffer, const void* data, VkDeviceSize size)
    {
        assert(dstBuffer != nullptr);
        assert(data != nullptr);
        assert(size > 0);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT; 
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
            | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer      stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAlloc = VK_NULL_HANDLE;
        VmaAllocationInfo stagingAllocInfo{};

        VK_CHECK(vmaCreateBuffer(
            m_allocator,
            &bufferInfo,
            &allocInfo,
            &stagingBuffer,
            &stagingAlloc,
            &stagingAllocInfo));

        memcpy(stagingAllocInfo.pMappedData, data, size);

        ImmediateSubmit([&](VkCommandBuffer cmd) {
            VkBufferCopy copyRegion{};
            copyRegion.srcOffset = 0;
            copyRegion.dstOffset = 0;
            copyRegion.size = size;
            vkCmdCopyBuffer(cmd, stagingBuffer, dstBuffer, 1, &copyRegion);
            });

        vmaDestroyBuffer(m_allocator, stagingBuffer, stagingAlloc);
    }

    bool Device::MemoryCreateImage(VkImage* outImage, VmaAllocation* outAllocation, VkExtent3D extent, VkFormat format, VkImageUsageFlags usage, uint32_t mipLevels, VkSampleCountFlagBits samples, const char* name)
    {
        VkImageCreateInfo imageInfo{};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.format = format;
        imageInfo.extent = extent;
        imageInfo.mipLevels = mipLevels;
        imageInfo.arrayLayers = 1;
        imageInfo.samples = samples;
        imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
        imageInfo.usage = usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        VK_CHECK(vmaCreateImage(
            m_allocator, &imageInfo, &allocInfo,
            outImage, outAllocation, nullptr));

        // Debug
        if (m_pfnSetDebugUtilsObjectNameEXT)
        {
            VkDebugUtilsObjectNameInfoEXT nameInfo{};
            nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
            nameInfo.objectType = VK_OBJECT_TYPE_IMAGE;
            nameInfo.objectHandle = reinterpret_cast<uint64_t>(*outImage);
            nameInfo.pObjectName = name;
            m_pfnSetDebugUtilsObjectNameEXT(m_device, &nameInfo);
        }
        return true;
    }

    void Device::MemoryDestroyImage(VkImage image, VmaAllocation allocation)
    {
        if (image != VK_NULL_HANDLE)
        {
            vmaDestroyImage(m_allocator, image, allocation);
        }
    }

    void Device::UploadDataToImage(
        VkImage             dstImage,
        VkExtent3D          extent,
        VkImageUsageFlags   usage,
        const void*         data,
        VkDeviceSize        size)
    {
        assert(dstImage != VK_NULL_HANDLE);
        assert(data != nullptr);
        assert(size > 0);

        VkBufferCreateInfo bufferInfo{};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

        VmaAllocationCreateInfo allocInfo{};
        allocInfo.usage = VMA_MEMORY_USAGE_CPU_TO_GPU;
        allocInfo.flags = VMA_ALLOCATION_CREATE_MAPPED_BIT
            | VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT;

        VkBuffer      stagingBuffer = VK_NULL_HANDLE;
        VmaAllocation stagingAlloc = VK_NULL_HANDLE;
        VmaAllocationInfo stagingInfo{};

        VK_CHECK(vmaCreateBuffer(m_allocator, &bufferInfo, &allocInfo,
            &stagingBuffer, &stagingAlloc, &stagingInfo));

        memcpy(stagingInfo.pMappedData, data, static_cast<size_t>(size));

        VkImageLayout finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        if (usage & VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT)
            finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
        else if (usage & VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT)
            finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

        ImmediateSubmit([&](VkCommandBuffer cmd) {

            VkImageMemoryBarrier2 barrier1{};
            barrier1.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier1.srcStageMask = VK_PIPELINE_STAGE_2_TOP_OF_PIPE_BIT;
            barrier1.srcAccessMask = VK_ACCESS_2_NONE;
            barrier1.dstStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
            barrier1.dstAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier1.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
            barrier1.newLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier1.image = dstImage;
            barrier1.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1
            };

            VkDependencyInfo depInfo1{};
            depInfo1.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            depInfo1.imageMemoryBarrierCount = 1;
            depInfo1.pImageMemoryBarriers = &barrier1;
            vkCmdPipelineBarrier2(cmd, &depInfo1);

            VkBufferImageCopy copyRegion{};
            copyRegion.bufferOffset = 0;
            copyRegion.bufferRowLength = 0;
            copyRegion.bufferImageHeight = 0;
            copyRegion.imageSubresource = {
                VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1
            };
            copyRegion.imageOffset = { 0, 0, 0 };
            copyRegion.imageExtent = extent;

            vkCmdCopyBufferToImage(cmd, stagingBuffer, dstImage,
                VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copyRegion);

            VkImageMemoryBarrier2 barrier2{};
            barrier2.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER_2;
            barrier2.srcStageMask = VK_PIPELINE_STAGE_2_COPY_BIT;
            barrier2.srcAccessMask = VK_ACCESS_2_TRANSFER_WRITE_BIT;
            barrier2.dstStageMask = VK_PIPELINE_STAGE_2_FRAGMENT_SHADER_BIT;
            barrier2.dstAccessMask = VK_ACCESS_2_SHADER_READ_BIT;
            barrier2.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier2.newLayout = finalLayout;
            barrier2.image = dstImage;
            barrier2.subresourceRange = {
                VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1
            };

            VkDependencyInfo depInfo2{};
            depInfo2.sType = VK_STRUCTURE_TYPE_DEPENDENCY_INFO;
            depInfo2.imageMemoryBarrierCount = 1;
            depInfo2.pImageMemoryBarriers = &barrier2;
            vkCmdPipelineBarrier2(cmd, &depInfo2);
            });

        vmaDestroyBuffer(m_allocator, stagingBuffer, stagingAlloc);
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

    void Device::CreateExtFunctions()
    {
        m_pfnSetDebugUtilsObjectNameEXT =
            reinterpret_cast<PFN_vkSetDebugUtilsObjectNameEXT>(
                vkGetDeviceProcAddr(m_device, "vkSetDebugUtilsObjectNameEXT"));
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