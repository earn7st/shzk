#pragma once

#include <optional>
#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <resource/ResourceHandle.h>

struct Transform
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

using NodeID = uint32_t;

struct Node
{
    static constexpr uint32_t InvalidNode = UINT32_MAX;

    std::string name;

    NodeID parent;
    std::vector<NodeID> children;

    Transform transform;
    std::optional<MeshHandle> mesh;
};

