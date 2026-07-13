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
        static VkImageView CreateImageView(
            VkDevice           device,
            VkImage& image,
            VkFormat           format,
            VkImageAspectFlags image_aspect_flags,
            VkImageViewType    view_type,
            uint32_t           layout_count,
            uint32_t           miplevels);

    };

}
