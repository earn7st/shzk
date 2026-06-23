#pragma once

#include <vma/vk_mem_alloc.h>

// Buffer
struct AllocatedBuffer
{
    VkBuffer buffer;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};

// Image
struct AllocatedImage
{
    vk::Image image;
    VmaAllocation allocation;
    VmaAllocationInfo info;
};