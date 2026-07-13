#pragma once

#include <glm/glm.hpp>

#include "vulkan/VulkanResource.h"

struct Material
{
    glm::vec4 baseColorFactor{ 1.0f, 1.0f, 1.0f, 1.0f };
    float     metallicFactor{ 1.0f };
    float     roughnessFactor{ 1.0f };
    float     normalScale{ 1.0f };
    float     occlusionStrength{ 1.0f };
    glm::vec3 emissiveFactor{ 0.0f, 0.0f, 0.0f };
    float     emissiveStrength{ 0.0f };
    float     alphaCutoff{ 0.5f };

    enum AlphaMode : int32_t
    {
        ALPHA_MODE_OPAQUE = 0,
        ALPHA_MODE_MASK = 1,
        ALPHA_MODE_BLEND = 2,
    };
    int32_t alphaMode{ ALPHA_MODE_OPAQUE };

    static constexpr uint32_t InvalidIdx = UINT32_MAX;
    uint32_t baseColorTextureIdx{ InvalidIdx };
    uint32_t normalTextureIdx{ InvalidIdx };
    uint32_t metallicRoughnessTextureIdx{ InvalidIdx };
    uint32_t occlusionTextureIdx{ InvalidIdx };
    uint32_t emissiveTextureIdx{ InvalidIdx };


    // GPU Side
    AllocatedBuffer materialUBO;
    vk::DescriptorSet descSet;
};