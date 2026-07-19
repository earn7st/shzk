#pragma once

#include <vulkan/vulkan.h>
#include <fmt/core.h>

namespace vkR::rhi
{
#define VK_CHECK(x)                                         \
    do {                                                    \
        VkResult err = x;                                   \
        if (err != VK_SUCCESS) {                            \
            fmt::println("[Vulkan Error] {}", int(err));    \
            abort();                                        \
        }                                                   \
    } while (0)

    class RHIUtil
    {
    public:
    // --- Debug ---
        static void SetBufferDebugName(
            PFN_vkSetDebugUtilsObjectNameEXT pfn,
            VkDevice device,
            VkBuffer buffer,
            const char* name);

    };

}
