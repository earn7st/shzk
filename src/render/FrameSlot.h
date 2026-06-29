#pragma once

#include <vulkan/vulkan.hpp>
#include <glm/glm.hpp>

#include "vulkan/VulkanResource.h"

struct FrameSlot
{
    vk::CommandPool commandPool;
    vk::CommandBuffer mainCommandBuffer;

    vk::Fence renderFence;
    vk::Semaphore swapchainSemaphore;
    vk::Semaphore renderSemaphore;

    AllocatedImage depthImage;
    vk::ImageView depthImageView;

    AllocatedBuffer frameDataUBO;
    vk::DescriptorSet frameDescriptorSet;
};
