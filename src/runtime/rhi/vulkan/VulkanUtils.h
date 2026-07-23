#pragma once
#include "runtime/log/Log.h"

#include <cassert>
    
#define VK_CHECK(expr)\
    do {\
        VkResult _vk_result = (expr);\
        if (_vk_result != VK_SUCCESS) {\
            SHZK_LOG_ERROR("Vulkan call failed: VkResult = {}",\
                static_cast<int>(_vk_result));\
            assert(false);\
        }\
    } while (0)