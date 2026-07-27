#pragma once
#include "runtime/log/Log.h"

#include <cassert>
#include <memory>
#include <string>
    
#define VK_CHECK(expr)\
    do {\
        VkResult _vk_result = (expr);\
        if (_vk_result != VK_SUCCESS) {\
            SHZK_LOG_ERROR("Vulkan call failed: VkResult = {}",\
                static_cast<int>(_vk_result));\
            assert(false);\
        }\
    } while (0)

#define VULKAN_RHI() std::static_pointer_cast<VulkanRHI>(RHI::Get())

namespace shzk
{
    namespace VulkanUtil
    {
        static std::string QueueFlagsToString(VkQueueFlags queueFlags)
        {
            std::string str = "";
            if (queueFlags & VK_QUEUE_GRAPHICS_BIT)          str.append("VK_QUEUE_GRAPHICS_BIT ");
            if (queueFlags & VK_QUEUE_COMPUTE_BIT)           str.append("VK_QUEUE_COMPUTE_BIT ");
            if (queueFlags & VK_QUEUE_TRANSFER_BIT)          str.append("VK_QUEUE_TRANSFER_BIT ");
            if (queueFlags & VK_QUEUE_SPARSE_BINDING_BIT)    str.append("VK_QUEUE_SPARSE_BINDING_BIT ");
            if (queueFlags & VK_QUEUE_PROTECTED_BIT)         str.append("VK_QUEUE_PROTECTED_BIT ");
            if (queueFlags & VK_QUEUE_VIDEO_DECODE_BIT_KHR)  str.append("VK_QUEUE_VIDEO_DECODE_BIT_KHR ");
            if (queueFlags & VK_QUEUE_OPTICAL_FLOW_BIT_NV)   str.append("VK_QUEUE_OPTICAL_FLOW_BIT_NV ");

            return  str;
        }

        static VkFormat RHIFormatToVkFormat(const RHIFormat& pixelFormat)
        {
            VkFormat format;

            switch (pixelFormat) {
            case FORMAT_UKNOWN:                format = VK_FORMAT_UNDEFINED;               break;

            case FORMAT_R8_SRGB:               format = VK_FORMAT_R8_SRGB;                 break;
            case FORMAT_R8G8_SRGB:             format = VK_FORMAT_R8G8_SRGB;               break;
            case FORMAT_R8G8B8_SRGB:           format = VK_FORMAT_R8G8B8_SRGB;             break;
            case FORMAT_R8G8B8A8_SRGB:         format = VK_FORMAT_R8G8B8A8_SRGB;           break;
            case FORMAT_B8G8R8A8_SRGB:         format = VK_FORMAT_B8G8R8A8_SRGB;           break;

            case FORMAT_R16_SFLOAT:            format = VK_FORMAT_R16_SFLOAT;              break;
            case FORMAT_R16G16_SFLOAT:         format = VK_FORMAT_R16G16_SFLOAT;           break;
            case FORMAT_R16G16B16_SFLOAT:      format = VK_FORMAT_R16G16B16_SFLOAT;        break;
            case FORMAT_R16G16B16A16_SFLOAT:   format = VK_FORMAT_R16G16B16A16_SFLOAT;     break;
            case FORMAT_R32_SFLOAT:            format = VK_FORMAT_R32_SFLOAT;              break;
            case FORMAT_R32G32_SFLOAT:         format = VK_FORMAT_R32G32_SFLOAT;           break;
            case FORMAT_R32G32B32_SFLOAT:      format = VK_FORMAT_R32G32B32_SFLOAT;        break;
            case FORMAT_R32G32B32A32_SFLOAT:   format = VK_FORMAT_R32G32B32A32_SFLOAT;     break;

            case FORMAT_R8_UNORM:             format = VK_FORMAT_R8_UNORM;                break;
            case FORMAT_R8G8_UNORM:           format = VK_FORMAT_R8G8_UNORM;              break;
            case FORMAT_R8G8B8_UNORM:         format = VK_FORMAT_R8G8B8_UNORM;            break;
            case FORMAT_R8G8B8A8_UNORM:       format = VK_FORMAT_R8G8B8A8_UNORM;          break;
            case FORMAT_R16_UNORM:            format = VK_FORMAT_R16_UNORM;               break;
            case FORMAT_R16G16_UNORM:         format = VK_FORMAT_R16G16_UNORM;            break;
            case FORMAT_R16G16B16_UNORM:      format = VK_FORMAT_R16G16B16_UNORM;         break;
            case FORMAT_R16G16B16A16_UNORM:   format = VK_FORMAT_R16G16B16A16_UNORM;      break;

            case FORMAT_R8_SNORM:             format = VK_FORMAT_R8_SNORM;                break;
            case FORMAT_R8G8_SNORM:           format = VK_FORMAT_R8G8_SNORM;              break;
            case FORMAT_R8G8B8_SNORM:         format = VK_FORMAT_R8G8B8_SNORM;            break;
            case FORMAT_R8G8B8A8_SNORM:       format = VK_FORMAT_R8G8B8A8_SNORM;          break;
            case FORMAT_R16_SNORM:            format = VK_FORMAT_R16_SNORM;               break;
            case FORMAT_R16G16_SNORM:         format = VK_FORMAT_R16G16_SNORM;            break;
            case FORMAT_R16G16B16_SNORM:      format = VK_FORMAT_R16G16B16_SNORM;         break;
            case FORMAT_R16G16B16A16_SNORM:   format = VK_FORMAT_R16G16B16A16_SNORM;      break;

            case FORMAT_R8_UINT:              format = VK_FORMAT_R8_UINT;                 break;
            case FORMAT_R8G8_UINT:            format = VK_FORMAT_R8G8_UINT;               break;
            case FORMAT_R8G8B8_UINT:          format = VK_FORMAT_R8G8B8_UINT;             break;
            case FORMAT_R8G8B8A8_UINT:        format = VK_FORMAT_R8G8B8A8_UINT;           break;
            case FORMAT_R16_UINT:             format = VK_FORMAT_R16_UINT;                break;
            case FORMAT_R16G16_UINT:          format = VK_FORMAT_R16G16_UINT;             break;
            case FORMAT_R16G16B16_UINT:       format = VK_FORMAT_R16G16B16_UINT;          break;
            case FORMAT_R16G16B16A16_UINT:    format = VK_FORMAT_R16G16B16A16_UINT;       break;
            case FORMAT_R32_UINT:             format = VK_FORMAT_R32_UINT;                break;
            case FORMAT_R32G32_UINT:          format = VK_FORMAT_R32G32_UINT;             break;
            case FORMAT_R32G32B32_UINT:       format = VK_FORMAT_R32G32B32_UINT;          break;
            case FORMAT_R32G32B32A32_UINT:    format = VK_FORMAT_R32G32B32A32_UINT;       break;

            case FORMAT_R8_SINT:              format = VK_FORMAT_R8_SINT;                 break;
            case FORMAT_R8G8_SINT:            format = VK_FORMAT_R8G8_SINT;               break;
            case FORMAT_R8G8B8_SINT:          format = VK_FORMAT_R8G8B8_SINT;             break;
            case FORMAT_R8G8B8A8_SINT:        format = VK_FORMAT_R8G8B8A8_SINT;           break;
            case FORMAT_R16_SINT:             format = VK_FORMAT_R16_SINT;                break;
            case FORMAT_R16G16_SINT:          format = VK_FORMAT_R16G16_SINT;             break;
            case FORMAT_R16G16B16_SINT:       format = VK_FORMAT_R16G16B16_SINT;          break;
            case FORMAT_R16G16B16A16_SINT:    format = VK_FORMAT_R16G16B16A16_SINT;       break;
            case FORMAT_R32_SINT:             format = VK_FORMAT_R32_SINT;                break;
            case FORMAT_R32G32_SINT:          format = VK_FORMAT_R32G32_SINT;             break;
            case FORMAT_R32G32B32_SINT:       format = VK_FORMAT_R32G32B32_SINT;          break;
            case FORMAT_R32G32B32A32_SINT:    format = VK_FORMAT_R32G32B32A32_SINT;       break;

            case FORMAT_D32_SFLOAT:           format = VK_FORMAT_D32_SFLOAT;              break;
            case FORMAT_D32_SFLOAT_S8_UINT:   format = VK_FORMAT_D32_SFLOAT_S8_UINT;      break;
            case FORMAT_D24_UNORM_S8_UINT:    format = VK_FORMAT_D24_UNORM_S8_UINT;       break;

            case FORMAT_A2R10G10B10_SNORM:    format = VK_FORMAT_A2R10G10B10_SNORM_PACK32;  break;
            case FORMAT_A2R10G10B10_UNORM:    format = VK_FORMAT_A2R10G10B10_UNORM_PACK32;  break;
            case FORMAT_A2R10G10B10_SINT:     format = VK_FORMAT_A2R10G10B10_SINT_PACK32;   break;
            case FORMAT_A2R10G10B10_UINT:     format = VK_FORMAT_A2R10G10B10_UINT_PACK32;   break;
            case FORMAT_B10G11R11_UFLOAT:     format = VK_FORMAT_B10G11R11_UFLOAT_PACK32;   break;
            case FORMAT_E5B9G9R9_UFLOAT:      format = VK_FORMAT_E5B9G9R9_UFLOAT_PACK32;    break;

            default:                          format = VK_FORMAT_UNDEFINED;               break;
            }

            return format;
        }

        static RHIFormat VkFormatToRHIFormat(const VkFormat& vkFormat)
        {
            RHIFormat format;

            switch (vkFormat) {
            case VK_FORMAT_UNDEFINED:            format = FORMAT_UKNOWN;                  break;

            case VK_FORMAT_R8_SRGB:              format = FORMAT_R8_SRGB;                 break;
            case VK_FORMAT_R8G8_SRGB:            format = FORMAT_R8G8_SRGB;               break;
            case VK_FORMAT_R8G8B8_SRGB:          format = FORMAT_R8G8B8_SRGB;             break;
            case VK_FORMAT_R8G8B8A8_SRGB:        format = FORMAT_R8G8B8A8_SRGB;           break;
            case VK_FORMAT_B8G8R8A8_SRGB:        format = FORMAT_B8G8R8A8_SRGB;           break;

            case VK_FORMAT_R16_SFLOAT:           format = FORMAT_R16_SFLOAT;              break;
            case VK_FORMAT_R16G16_SFLOAT:        format = FORMAT_R16G16_SFLOAT;           break;
            case VK_FORMAT_R16G16B16_SFLOAT:     format = FORMAT_R16G16B16_SFLOAT;        break;
            case VK_FORMAT_R16G16B16A16_SFLOAT:  format = FORMAT_R16G16B16A16_SFLOAT;     break;
            case VK_FORMAT_R32_SFLOAT:           format = FORMAT_R32_SFLOAT;              break;
            case VK_FORMAT_R32G32_SFLOAT:        format = FORMAT_R32G32_SFLOAT;           break;
            case VK_FORMAT_R32G32B32_SFLOAT:     format = FORMAT_R32G32B32_SFLOAT;        break;
            case VK_FORMAT_R32G32B32A32_SFLOAT:  format = FORMAT_R32G32B32A32_SFLOAT;     break;

            case VK_FORMAT_R8_UNORM:             format = FORMAT_R8_UNORM;                break;
            case VK_FORMAT_R8G8_UNORM:           format = FORMAT_R8G8_UNORM;              break;
            case VK_FORMAT_R8G8B8_UNORM:         format = FORMAT_R8G8B8_UNORM;            break;
            case VK_FORMAT_R8G8B8A8_UNORM:       format = FORMAT_R8G8B8A8_UNORM;          break;
            case VK_FORMAT_R16_UNORM:            format = FORMAT_R16_UNORM;               break;
            case VK_FORMAT_R16G16_UNORM:         format = FORMAT_R16G16_UNORM;            break;
            case VK_FORMAT_R16G16B16_UNORM:      format = FORMAT_R16G16B16_UNORM;         break;
            case VK_FORMAT_R16G16B16A16_UNORM:   format = FORMAT_R16G16B16A16_UNORM;      break;

            case VK_FORMAT_R8_SNORM:             format = FORMAT_R8_SNORM;                break;
            case VK_FORMAT_R8G8_SNORM:           format = FORMAT_R8G8_SNORM;              break;
            case VK_FORMAT_R8G8B8_SNORM:         format = FORMAT_R8G8B8_SNORM;            break;
            case VK_FORMAT_R8G8B8A8_SNORM:       format = FORMAT_R8G8B8A8_SNORM;          break;
            case VK_FORMAT_R16_SNORM:            format = FORMAT_R16_SNORM;               break;
            case VK_FORMAT_R16G16_SNORM:         format = FORMAT_R16G16_SNORM;            break;
            case VK_FORMAT_R16G16B16_SNORM:      format = FORMAT_R16G16B16_SNORM;         break;
            case VK_FORMAT_R16G16B16A16_SNORM:   format = FORMAT_R16G16B16A16_SNORM;      break;

            case VK_FORMAT_R8_UINT:              format = FORMAT_R8_UINT;                 break;
            case VK_FORMAT_R8G8_UINT:            format = FORMAT_R8G8_UINT;               break;
            case VK_FORMAT_R8G8B8_UINT:          format = FORMAT_R8G8B8_UINT;             break;
            case VK_FORMAT_R8G8B8A8_UINT:        format = FORMAT_R8G8B8A8_UINT;           break;
            case VK_FORMAT_R16_UINT:             format = FORMAT_R16_UINT;                break;
            case VK_FORMAT_R16G16_UINT:          format = FORMAT_R16G16_UINT;             break;
            case VK_FORMAT_R16G16B16_UINT:       format = FORMAT_R16G16B16_UINT;          break;
            case VK_FORMAT_R16G16B16A16_UINT:    format = FORMAT_R16G16B16A16_UINT;       break;
            case VK_FORMAT_R32_UINT:             format = FORMAT_R32_UINT;                break;
            case VK_FORMAT_R32G32_UINT:          format = FORMAT_R32G32_UINT;             break;
            case VK_FORMAT_R32G32B32_UINT:       format = FORMAT_R32G32B32_UINT;          break;
            case VK_FORMAT_R32G32B32A32_UINT:    format = FORMAT_R32G32B32A32_UINT;       break;

            case VK_FORMAT_R8_SINT:              format = FORMAT_R8_SINT;                 break;
            case VK_FORMAT_R8G8_SINT:            format = FORMAT_R8G8_SINT;               break;
            case VK_FORMAT_R8G8B8_SINT:          format = FORMAT_R8G8B8_SINT;             break;
            case VK_FORMAT_R8G8B8A8_SINT:        format = FORMAT_R8G8B8A8_SINT;           break;
            case VK_FORMAT_R16_SINT:             format = FORMAT_R16_SINT;                break;
            case VK_FORMAT_R16G16_SINT:          format = FORMAT_R16G16_SINT;             break;
            case VK_FORMAT_R16G16B16_SINT:       format = FORMAT_R16G16B16_SINT;          break;
            case VK_FORMAT_R16G16B16A16_SINT:    format = FORMAT_R16G16B16A16_SINT;       break;
            case VK_FORMAT_R32_SINT:             format = FORMAT_R32_SINT;                break;
            case VK_FORMAT_R32G32_SINT:          format = FORMAT_R32G32_SINT;             break;
            case VK_FORMAT_R32G32B32_SINT:       format = FORMAT_R32G32B32_SINT;          break;
            case VK_FORMAT_R32G32B32A32_SINT:    format = FORMAT_R32G32B32A32_SINT;       break;

            case VK_FORMAT_D32_SFLOAT:           format = FORMAT_D32_SFLOAT;              break;
            case VK_FORMAT_D32_SFLOAT_S8_UINT:   format = FORMAT_D32_SFLOAT_S8_UINT;      break;
            case VK_FORMAT_D24_UNORM_S8_UINT:    format = FORMAT_D24_UNORM_S8_UINT;       break;

            case VK_FORMAT_A2R10G10B10_SNORM_PACK32:    format = FORMAT_A2R10G10B10_SNORM;  break;
            case VK_FORMAT_A2R10G10B10_UNORM_PACK32:    format = FORMAT_A2R10G10B10_UNORM;  break;
            case VK_FORMAT_A2R10G10B10_SINT_PACK32:     format = FORMAT_A2R10G10B10_SINT;   break;
            case VK_FORMAT_A2R10G10B10_UINT_PACK32:     format = FORMAT_A2R10G10B10_UINT;   break;
            case VK_FORMAT_B10G11R11_UFLOAT_PACK32:     format = FORMAT_B10G11R11_UFLOAT;   break;
            case VK_FORMAT_E5B9G9R9_UFLOAT_PACK32:      format = FORMAT_E5B9G9R9_UFLOAT;    break;

            default:                             format = FORMAT_UKNOWN;                   break;
            }

            return format;
        }
    }
}