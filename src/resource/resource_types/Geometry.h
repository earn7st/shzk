#pragma once

#include <vma/vk_mem_alloc.h>

#include "vulkan/VulkanResource.h"

struct Geometry
{
    AllocatedBuffer vertexBuffer;
    AllocatedBuffer indexBuffer;

    uint32_t vertexCount;
    uint32_t indexCount;
};