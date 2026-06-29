#pragma once

#include <vulkan/vulkan.hpp>

#include "render/FrameUniforms.h"

struct DrawCommand
{
    glm::mat4 modelMatrix;
    vk::Buffer vertexBuffer;
    vk::Buffer indexBuffer;
    uint32_t indexCount;

    vk::DescriptorSet materialSet;

    uint32_t sortKey = 0;
};

struct RenderView
{
    FrameUniforms frameData{};

    std::vector<DrawCommand> opaqueDraws;
    std::vector<DrawCommand> transparentDraws;
};
