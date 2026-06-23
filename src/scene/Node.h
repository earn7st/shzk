#pragma once

#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include <resource/ResourceHandle.h>

struct Transform
{
    glm::vec3 position;
    glm::quat rotation;
    glm::vec3 scale;
};

struct Node
{
    Node* parent;
    std::vector<Node*> children;

    Transform transform;
    std::optional<MeshHandle> mesh;
};