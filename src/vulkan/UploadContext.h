#pragma once

#include <vulkan/vulkan.hpp>

struct VulkanContext;

struct UploadContext
{
	vk::Fence uploadFence;
	vk::CommandPool commandPool;
	vk::CommandBuffer commandBuffer;
};

std::unique_ptr<UploadContext> CreateUploadContext(const VulkanContext& vulkanContext);