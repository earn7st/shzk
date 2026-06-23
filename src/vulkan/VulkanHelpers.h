#pragma once

#include <iostream>
#include <functional>
#include <vma/vk_mem_alloc.h>
#include <fmt/core.h>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>

#include "resource/cpu_types/GeometryData.h"
#include "resource/resource_types/Geometry.h"
#include "vulkan/VulkanResource.h"

struct VulkanContext;
struct UploadContext;

#define VK_CHECK(x)                                                     \
    do {                                                                \
        vk::Result err = static_cast<vk::Result>(x);                                              \
        if (err < vk::Result::eSuccess) {                                                      \
            fmt::println("[Vulkan Error] {}", vk::to_string(err)); \
            abort();                                                    \
        }                                                               \
    } while (0)


// Geometry
Geometry CreateGeometry(const VulkanContext& vulkanContext, const UploadContext& uploadContext, const GeometryData& data);
void DestroyGeometry(const VulkanContext& vulkanContext, Geometry& geometry);

// Buffer
AllocatedBuffer CreateBuffer(VmaAllocator& allocator, size_t allocSize, VkBufferUsageFlags usage, VmaMemoryUsage memoryUsage);

// Image
AllocatedImage CreateImage();

// Immediate Submit
void ImmediateSubmit(const VulkanContext& vulkanContext, const UploadContext& uploadContext, std::function<void(vk::CommandBuffer cmd)> && function);