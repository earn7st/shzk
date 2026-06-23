#include "UploadContext.h"

#include <VkBootstrap.h>
#include <vulkan/vulkan.hpp>

#include "vulkan/VulkanContext.h"

std::unique_ptr<UploadContext> CreateUploadContext(const VulkanContext& vulkanContext)
{
    auto uploadContext = std::make_unique<UploadContext>();

    vk::CommandPoolCreateInfo commandPoolInfo{};
    commandPoolInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolInfo.queueFamilyIndex = vulkanContext.graphicsQueueFamily;

    uploadContext->commandPool = vulkanContext.device.createCommandPool(commandPoolInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocInfo{};
    commandBufferAllocInfo.commandPool = uploadContext->commandPool;
    commandBufferAllocInfo.level = vk::CommandBufferLevel::ePrimary;
    commandBufferAllocInfo.commandBufferCount = 1;

    auto commandBuffers = vulkanContext.device.allocateCommandBuffers(commandBufferAllocInfo);

    uploadContext->commandBuffer = commandBuffers.front();

    vk::FenceCreateInfo fenceCreateInfo{};
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;

    uploadContext->uploadFence = vulkanContext.device.createFence(fenceCreateInfo);

    return uploadContext;
}