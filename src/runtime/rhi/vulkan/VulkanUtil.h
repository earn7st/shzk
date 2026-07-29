#pragma once
#include "runtime/log/Log.h"

#include <cassert>
#include <memory>
#include <string>

#include <vma/vk_mem_alloc.h>
    
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

        static VmaMemoryUsage MemoryUsageToVma(MemoryUsage memoryUsage)
        {
            VmaMemoryUsage usage;
            switch (memoryUsage) {
            case MemoryUsage::Unknown:      usage = VMA_MEMORY_USAGE_UNKNOWN;       break;
            case MemoryUsage::GPU_Only:     usage = VMA_MEMORY_USAGE_GPU_ONLY;      break;
            case MemoryUsage::CPU_Only:     usage = VMA_MEMORY_USAGE_CPU_ONLY;      break;
            case MemoryUsage::CPU_To_GPU:   usage = VMA_MEMORY_USAGE_CPU_TO_GPU;    break;
            case MemoryUsage::GPU_To_CPU:   usage = VMA_MEMORY_USAGE_GPU_TO_CPU;    break;
            default:                        usage = VMA_MEMORY_USAGE_UNKNOWN;       break;
            }

            return usage;
        }

        static VkBufferUsageFlags ResourceTypeToBufferUsage(ResourceType type)
        {
            VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT;
            if (type & RESOURCE_TYPE_UNIFORM_BUFFER)        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            if (type & RESOURCE_TYPE_RW_BUFFER)             usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            if (type & RESOURCE_TYPE_BUFFER)                usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            if (type & RESOURCE_TYPE_INDEX_BUFFER)          usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
            if (type & RESOURCE_TYPE_VERTEX_BUFFER)         usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;
            if (type & RESOURCE_TYPE_INDIRECT_BUFFER)       usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            if (type & RESOURCE_TYPE_RAY_TRACING)           usage |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

            return usage;
        }

        static VkImageUsageFlags ResourceTypeToImageUsage(ResourceType type)
        {
            VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            if (type & RESOURCE_TYPE_TEXTURE)               usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            if (type & RESOURCE_TYPE_RW_TEXTURE)            usage |= VK_IMAGE_USAGE_STORAGE_BIT;

            return usage;
        }

    }
}