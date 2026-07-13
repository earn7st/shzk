#pragma once

#include <glm/glm.hpp>

struct FrameUniforms
{
    glm::mat4 view;
    glm::mat4 proj;
    glm::mat4 viewProj;
    glm::mat4 invViewProj;
    glm::vec4 cameraPosition;
    glm::vec4 time;
    glm::vec4 resolution;
    glm::vec4 ambientColor;
    glm::vec4 sunDirection;
    glm::vec4 sunColor;
};

static_assert(sizeof(FrameUniforms) == 352, "FrameUniforms must match shader layout");