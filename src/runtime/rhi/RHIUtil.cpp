#include "RHIUtil.h"

#include <stdexcept>
#include <fmt/core.h>

namespace vkR::rhi
{
    void RHIUtil::SetBufferDebugName(
        PFN_vkSetDebugUtilsObjectNameEXT pfn,
        VkDevice device,
        VkBuffer buffer,
        const char* name)
    {
        if (!pfn) return;

        VkDebugUtilsObjectNameInfoEXT nameInfo{};
        nameInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_OBJECT_NAME_INFO_EXT;
        nameInfo.objectType = VK_OBJECT_TYPE_BUFFER;
        nameInfo.objectHandle = reinterpret_cast<uint64_t>(buffer);
        nameInfo.pObjectName = name;
        pfn(device, &nameInfo);
    }
}