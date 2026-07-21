// NOTE: No #pragma once — VMA is designed to be included multiple times
// in the same translation unit: once for declarations, then again after
// #define VMA_IMPLEMENTATION for the implementation.

// This wrapper allows the existing <vma/vk_mem_alloc.h> include to resolve
// against the VMA submodule at extern/VulkanMemoryAllocator.
#include "../VulkanMemoryAllocator/include/vk_mem_alloc.h"
