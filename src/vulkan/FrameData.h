#pragma once

struct FrameData
{
    vk::CommandPool commandPool;
    vk::CommandBuffer mainCommandBuffer;

    vk::Fence renderFence;

    vk::Semaphore swapchainSemaphore;
    vk::Semaphore renderSemaphore;
};
