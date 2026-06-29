#pragma once

#include <iostream>
#include <functional>
#include <vulkan/vulkan.hpp>
#include <vma/vk_mem_alloc.h>
#include <fmt/core.h>

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
AllocatedBuffer CreateBuffer(VmaAllocator& allocator, size_t allocSize, vk::BufferUsageFlags usage, VmaMemoryUsage memoryUsage);
void DestroyBuffer(VmaAllocator & allocator, AllocatedBuffer & buffer);

// Image
AllocatedImage CreateImage(
    VmaAllocator allocator, 
    vk::Extent3D extent,
    vk::Format format,
    vk::ImageUsageFlags flags,
    VmaMemoryUsage usage);

vk::ImageView CreateImageView(
    vk::Device device,
    vk::Image image,
    vk::Format format,
    vk::ImageAspectFlags flags);

void DestroyImage(VmaAllocator& allocator, AllocatedImage& image);

// Immediate Submit
void ImmediateSubmit(const VulkanContext& vulkanContext, const UploadContext& uploadContext, std::function<void(vk::CommandBuffer cmd)> && function);

// TransitionImageLayout
void TransitionImageLayout(
    vk::CommandBuffer cmd,
    vk::Image image,
    vk::ImageLayout oldLayout,
    vk::ImageLayout newLayout,
    vk::ImageAspectFlags aspectFlags);

// Shader
vk::ShaderModule LoadShaderModule(vk::Device device, const std::string & filePath);

// Sampler
vk::Sampler CreateSampler(
    vk::Device device,
    vk::Filter magFilter,
    vk::Filter minFilter,
    vk::SamplerAddressMode addressModeU,
    vk::SamplerAddressMode addressModeV);