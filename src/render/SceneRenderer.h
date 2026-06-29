#pragma once

#include <glm/glm.hpp>

#include "render/RenderView.h"

class Scene;
class ResourceManager;
class Renderer;
struct FrameUniforms;

class SceneRenderer
{
public:
    SceneRenderer() = delete;
    SceneRenderer(Scene* scene, ResourceManager* resourceManager);

    RenderView BuildRenderView(const FrameUniforms& frameData);

private:
    Scene* _scene;
    ResourceManager* _resourceManager;

    void CollectDrawsRecursive(
        uint32_t nodeIndex,
        const glm::mat4& parentWorld,
        RenderView& view);

    void CollectDrawsForNode(
        const struct Node& node,
        const glm::mat4& worldMatrix,
        RenderView& view);
};
