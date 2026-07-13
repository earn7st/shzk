#include "SceneRenderer.h"

#include <glm/glm.hpp>

#include "Scene/Scene.h"
#include "scene/Node.h"
#include "render/FrameUniforms.h"
#include "render/RenderView.h"
#include "render/Renderer.h"
#include "resource/ResourceManager.h"
#include "resource/resource_types/Mesh.h"
#include "resource/resource_types/Geometry.h"
#include "resource/resource_types/Material.h"

SceneRenderer::SceneRenderer(
    Scene* scene,
    ResourceManager* resourceManager) :
    _scene(scene),
    _resourceManager(resourceManager)
{ }

RenderView SceneRenderer::BuildRenderView(const FrameUniforms& frameData)
{
    RenderView view;
    view.frameData = frameData;

    for (uint32_t rootIdx : _scene->rootNodes)
    {
        CollectDrawsRecursive(rootIdx, glm::mat4(1.0f), view);
    }

    return view;
}

void SceneRenderer::CollectDrawsRecursive(
    uint32_t nodeIndex,
    const glm::mat4& parentWorld,
    RenderView& view)
{
    const Node& node = _scene->nodes[nodeIndex];

    const auto& t = node.transform;
    glm::mat4 localMatrix = glm::translate(glm::mat4(1.0f), t.position)
                            * glm::mat4_cast(t.rotation)
                            * glm::scale(glm::mat4(1.0f), t.scale);
    glm::mat4 worldMatrix = parentWorld * localMatrix;

    if (node.mesh.has_value())
    {
        CollectDrawsForNode(node, worldMatrix, view);
    }

    for (uint32_t childIdx : node.children)
    {
        CollectDrawsRecursive(childIdx, worldMatrix, view);
    }
}

void SceneRenderer::CollectDrawsForNode(
    const struct Node& node,
    const glm::mat4& worldMatrix,
    RenderView& view)
{
    const Mesh& mesh = _resourceManager->meshes[node.mesh->id];

    for (const auto& primitive : mesh.primitives)
    {
        if (!primitive.geometry.IsValid()) continue;

        const Geometry& geo = _resourceManager->geometries[primitive.geometry.id];

        DrawCommand cmd{};
        cmd.modelMatrix  = worldMatrix;
        cmd.vertexBuffer = geo.vertexBuffer.buffer;
        cmd.indexBuffer  = geo.indexBuffer.buffer;
        cmd.indexCount   = geo.indexCount;

        if (primitive.material.IsValid())
        {
            const Material& mat = _resourceManager->materials[primitive.material.id];
            cmd.materialSet = mat.descSet;
        }
        else
        {
            cmd.materialSet = _resourceManager->defaultMaterial.descSet;
        }

        view.opaqueDraws.push_back(cmd);
    }
}
