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
            case FORMAT_B8G8R8A8_UNORM:       format = VK_FORMAT_B8G8R8A8_UNORM;          break;
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
            case VK_FORMAT_B8G8R8A8_UNORM:       format = FORMAT_B8G8R8A8_UNORM;          break;
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
            case MemoryUsage::GPUOnly:      usage = VMA_MEMORY_USAGE_GPU_ONLY;      break;
            case MemoryUsage::CPUOnly:      usage = VMA_MEMORY_USAGE_CPU_ONLY;      break;
            case MemoryUsage::CPUToGPU:     usage = VMA_MEMORY_USAGE_CPU_TO_GPU;    break;
            case MemoryUsage::GPUToCPU:     usage = VMA_MEMORY_USAGE_GPU_TO_CPU;    break;
            default:                        usage = VMA_MEMORY_USAGE_UNKNOWN;       break;
            }

            return usage;
        }

        static VkImageAspectFlags TextureAspectToVk(TextureAspectFlags flags)
        {
            VkImageAspectFlags aspectFlags = 0;
            if (flags & TEXTURE_ASPECT_COLOR)                aspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
            if (flags & TEXTURE_ASPECT_DEPTH)                aspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
            if (flags & TEXTURE_ASPECT_STENCIL)              aspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
            return aspectFlags;
        }

        static VkBufferUsageFlags ResourceTypeToVkBufferUsage(ResourceType type)
        {
            VkBufferUsageFlags usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT /*| VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT*/;    // bindless
            if (type & RESOURCE_TYPE_UNIFORM_BUFFER)        usage |= VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
            if (type & RESOURCE_TYPE_RW_BUFFER)             usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            if (type & RESOURCE_TYPE_BUFFER)                usage |= VK_BUFFER_USAGE_STORAGE_BUFFER_BIT;
            if (type & RESOURCE_TYPE_INDEX_BUFFER)          usage |= VK_BUFFER_USAGE_INDEX_BUFFER_BIT /* | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR*/;  // raytracing
            if (type & RESOURCE_TYPE_VERTEX_BUFFER)         usage |= VK_BUFFER_USAGE_VERTEX_BUFFER_BIT /*| VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR*/;
            if (type & RESOURCE_TYPE_INDIRECT_BUFFER)       usage |= VK_BUFFER_USAGE_INDIRECT_BUFFER_BIT;
            // if (type & RESOURCE_TYPE_RAY_TRACING)           usage |= VK_BUFFER_USAGE_SHADER_BINDING_TABLE_BIT_KHR | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_STORAGE_BIT_KHR | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR;

            return usage;
        }

        static VkImageUsageFlags ResourceTypeToVkImageUsage(ResourceType type)
        {
            VkImageUsageFlags usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT;
            if (type & RESOURCE_TYPE_TEXTURE)               usage |= VK_IMAGE_USAGE_SAMPLED_BIT;
            if (type & RESOURCE_TYPE_RW_TEXTURE)            usage |= VK_IMAGE_USAGE_STORAGE_BIT;

            return usage;
        }

        static VkDescriptorType ResourceTypeToVkDescriptorType(ResourceType resourceType)
        {
            VkDescriptorType descriptorType;
            switch (resourceType) {
            case RESOURCE_TYPE_SAMPLER:                 descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER;                        break;
            case RESOURCE_TYPE_TEXTURE_CUBE:            descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;                  break;
            case RESOURCE_TYPE_TEXTURE:                 descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE;                  break;
            case RESOURCE_TYPE_RW_TEXTURE:              descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;                  break;
            case RESOURCE_TYPE_COMBINED_IMAGE_SAMPLER:  descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;         break;
            case RESOURCE_TYPE_BUFFER:                  descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;                 break;
            case RESOURCE_TYPE_RW_BUFFER:               descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;                 break;
            case RESOURCE_TYPE_UNIFORM_BUFFER:          descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;                 break;
            case RESOURCE_TYPE_TEXEL_BUFFER:            descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER;           break;
            case RESOURCE_TYPE_RW_TEXEL_BUFFER:         descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER;           break;
            // case RESOURCE_TYPE_RAY_TRACING:             descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;     break;
            default:                                    SHZK_LOG_ERROR("Unsupported resource type!");
            }
            return descriptorType;
        }

        static VkAccessFlags ResourceStateToVkAccessFlags(RHIResourceState state)
        {
            VkAccessFlags accessFlags = VK_ACCESS_NONE;
            switch (state) {
            case RHIResourceState::Undefined:                   accessFlags = VK_ACCESS_NONE;                                           break;
            case RHIResourceState::Common:                      accessFlags = VK_ACCESS_NONE;                                           break;
            case RHIResourceState::TransferSrc:                 accessFlags = VK_ACCESS_TRANSFER_READ_BIT;                              break;
            case RHIResourceState::TransferDst:                 accessFlags = VK_ACCESS_TRANSFER_WRITE_BIT;                             break;
            case RHIResourceState::VertexBuffer:                accessFlags = VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT;                      break;
            case RHIResourceState::IndexBuffer:                 accessFlags = VK_ACCESS_INDEX_READ_BIT;                                 break;
            case RHIResourceState::ColorAttachment:             accessFlags = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;                     break;
            case RHIResourceState::DepthStencilAttachment:      accessFlags = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;             break;
            case RHIResourceState::UnorderedAccess:             accessFlags = VK_ACCESS_SHADER_READ_BIT | VK_ACCESS_SHADER_WRITE_BIT;   break;
            case RHIResourceState::ShaderResource:              accessFlags = VK_ACCESS_SHADER_READ_BIT;                                break;
                // case RHIResourceState::IndirectArgument:            accessFlags = VK_ACCESS_INDIRECT_COMMAND_READ_BIT;                      break;
            case RHIResourceState::Present:                     accessFlags = VK_ACCESS_NONE;                                           break;
                // case RHIResourceState::AccelerationStructure:         accessFlags = VK_ACCESS_ACCELERATION_STRUCTURE_READ_BIT_KHR | VK_ACCESS_ACCELERATION_STRUCTURE_WRITE_BIT_KHR;   break;
            default:                                            SHZK_LOG_ERROR("ResourceState Unsupported!");
            }
            return accessFlags;
        }

        static VkImageLayout ResourceStateToVkImageLayout(RHIResourceState state)
        {
            VkImageLayout layout = VK_IMAGE_LAYOUT_UNDEFINED;
            switch (state) {
            case RHIResourceState::Undefined:                       layout = VK_IMAGE_LAYOUT_UNDEFINED;                         break;
            case RHIResourceState::Common:                          layout = VK_IMAGE_LAYOUT_GENERAL;                           break;
            case RHIResourceState::TransferSrc:                     layout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;              break;
            case RHIResourceState::TransferDst:                     layout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;              break;
            case RHIResourceState::ColorAttachment:                 layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;          break;
            case RHIResourceState::DepthStencilAttachment:          layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;  break;
            case RHIResourceState::UnorderedAccess:                 layout = VK_IMAGE_LAYOUT_GENERAL;                           break;
            case RHIResourceState::ShaderResource:                  layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;          break;
            case RHIResourceState::Present:                         layout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;                   break;
            default:                                            SHZK_LOG_ERROR("ResourceState Unsupported!");
            }
            return layout;
        }

        static VkPipelineStageFlags VkAccessFlagsToVkPipelineStageFlags(VkAccessFlags accessFlags)
        {
            VkPipelineStageFlags flags = 0;

            if (accessFlags & VK_ACCESS_INDIRECT_COMMAND_READ_BIT)      flags |= VK_PIPELINE_STAGE_DRAW_INDIRECT_BIT;

            if (accessFlags & (VK_ACCESS_INDEX_READ_BIT |
                VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT))                   flags |= VK_PIPELINE_STAGE_VERTEX_INPUT_BIT;

            if (accessFlags & (VK_ACCESS_UNIFORM_READ_BIT |
                VK_ACCESS_SHADER_READ_BIT |
                VK_ACCESS_SHADER_WRITE_BIT))                            flags |=    VK_PIPELINE_STAGE_VERTEX_SHADER_BIT |
                                                                                    /*VK_PIPELINE_STAGE_TESSELLATION_CONTROL_SHADER_BIT |
                                                                                    VK_PIPELINE_STAGE_TESSELLATION_EVALUATION_SHADER_BIT |
                                                                                    VK_PIPELINE_STAGE_GEOMETRY_SHADER_BIT | */
                                                                                    VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT |
                                                                                    VK_PIPELINE_STAGE_COMPUTE_SHADER_BIT /* |
                                                                                    VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR*/;

            if (accessFlags & VK_ACCESS_INPUT_ATTACHMENT_READ_BIT)      flags |= VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

            if (accessFlags & (VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT |
                VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT))          flags |= VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT |
                VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

            if (accessFlags & (VK_ACCESS_COLOR_ATTACHMENT_READ_BIT |
                VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT))                  flags |= VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

            if (accessFlags & (VK_ACCESS_TRANSFER_READ_BIT |
                VK_ACCESS_TRANSFER_WRITE_BIT))                          flags |= VK_PIPELINE_STAGE_TRANSFER_BIT;

            if (accessFlags & (VK_ACCESS_HOST_READ_BIT |
                VK_ACCESS_HOST_WRITE_BIT))                              flags |= VK_PIPELINE_STAGE_HOST_BIT;


            if (flags == 0) flags = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            return flags;
        }

        static VkExtent2D ExtentToVk(const Extent2D& extent)
        {
            return VkExtent2D(extent.width, extent.height);
        }

        static VkExtent3D ExtentToVk(const Extent3D& extent)
        {
            return VkExtent3D(extent.width, extent.height, extent.depth);
        }

        static VkImageSubresourceRange SubresourceToVk(const TextureSubresourceRange& subresource)
        {

            return VkImageSubresourceRange(
                TextureAspectToVk(subresource.aspect),
                subresource.baseMipLevel,
                subresource.levelCount,
                subresource.baseArrayLayer,
                subresource.layerCount);
        }

        static VkImageSubresourceLayers SubresourceToVk(const TextureSubresourceLayers& subresource)
        {
            return VkImageSubresourceLayers(
                TextureAspectToVk(subresource.aspect),
                subresource.mipLevel,
                subresource.baseArrayLayer,
                subresource.layerCount);
        }

        static VkImageViewType TextureViewTypeToVk(const TextureViewType& type)
        {
            VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_MAX_ENUM;
            switch (type)
            {
            case TextureViewType::View1D:           viewType = VK_IMAGE_VIEW_TYPE_1D; break;
            case TextureViewType::View2D:           viewType = VK_IMAGE_VIEW_TYPE_2D; break;
            case TextureViewType::View3D:           viewType = VK_IMAGE_VIEW_TYPE_3D; break;
            case TextureViewType::ViewCube:         viewType = VK_IMAGE_VIEW_TYPE_CUBE; break;
            case TextureViewType::View1DArray:      viewType = VK_IMAGE_VIEW_TYPE_1D_ARRAY; break;
            case TextureViewType::View2DArray:      viewType = VK_IMAGE_VIEW_TYPE_2D_ARRAY; break;
            case TextureViewType::ViewCubeArray:    viewType = VK_IMAGE_VIEW_TYPE_CUBE_ARRAY; break;
            default:                                SHZK_LOG_ERROR("TextureViewType Unsupported!");
            }
            return viewType;
        }

        static VkImageAspectFlags TextureAspectFlagsToVk(const TextureAspectFlags& aspectFlags)
        {
            VkImageAspectFlags vkAspectFlags = 0;
            if (aspectFlags & TEXTURE_ASPECT_COLOR)      vkAspectFlags |= VK_IMAGE_ASPECT_COLOR_BIT;
            if (aspectFlags & TEXTURE_ASPECT_DEPTH)      vkAspectFlags |= VK_IMAGE_ASPECT_DEPTH_BIT;
            if (aspectFlags & TEXTURE_ASPECT_STENCIL)    vkAspectFlags |= VK_IMAGE_ASPECT_STENCIL_BIT;
            return vkAspectFlags;
        }

        static VkShaderStageFlags ShaderFrequencyToVkStageFlags(ShaderFrequency frequency)
        {
            VkShaderStageFlags stageFlags;
            if (frequency & SHADER_FREQUENCY_COMPUTE)        stageFlags |= VK_SHADER_STAGE_COMPUTE_BIT;
            if (frequency & SHADER_FREQUENCY_VERTEX)         stageFlags |= VK_SHADER_STAGE_VERTEX_BIT;
            if (frequency & SHADER_FREQUENCY_FRAGMENT)       stageFlags |= VK_SHADER_STAGE_FRAGMENT_BIT;
            if (frequency & SHADER_FREQUENCY_GEOMETRY)       stageFlags |= VK_SHADER_STAGE_GEOMETRY_BIT;
            return stageFlags;
        }

        static VkShaderStageFlagBits ShaderFrequencyToVkStageFlagBits(ShaderFrequency frequency)
        {
            if (frequency & SHADER_FREQUENCY_COMPUTE)        return VK_SHADER_STAGE_COMPUTE_BIT;
            if (frequency & SHADER_FREQUENCY_VERTEX)         return VK_SHADER_STAGE_VERTEX_BIT;
            if (frequency & SHADER_FREQUENCY_FRAGMENT)       return VK_SHADER_STAGE_FRAGMENT_BIT;
            if (frequency & SHADER_FREQUENCY_GEOMETRY)       return VK_SHADER_STAGE_GEOMETRY_BIT;
            return VK_SHADER_STAGE_ALL;
        }

        static VkFilter FilterTypeToVk(FilterType type)
        {
            VkFilter filter;
            switch (type) {
            case FilterType::Nearest:   filter = VK_FILTER_NEAREST;     break;
            case FilterType::Linear:    filter = VK_FILTER_LINEAR;      break;
            default:                    filter = VK_FILTER_LINEAR;      break;
            }
            return  filter;
        }
    }
}