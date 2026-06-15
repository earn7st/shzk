#include "scene/Scene.h"

void SceneNode::AddChild(const std::shared_ptr<SceneNode>& child)
{
	if (!child) {
		return;
	}

	child->parent = shared_from_this();
	children.push_back(child);
}

void SceneNode::RefreshTransform(const glm::mat4& parentTransform)
{
	worldTransform = parentTransform * localTransform;

	for (const auto& child : children) {
		child->RefreshTransform(worldTransform);
	}
}

void SceneNode::CollectDraws(DrawContext& context) const
{
	for (const auto& child : children) {
		child->CollectDraws(context);
	}
}

void MeshNode::CollectDraws(DrawContext& context) const
{
	if (mesh) {
		for (uint32_t surfaceIndex = 0; surfaceIndex < mesh->surfaces.size(); ++surfaceIndex) {
			context.renderObjects.push_back(RenderObject{
				.mesh = mesh,
				.surfaceIndex = surfaceIndex,
				.transform = worldTransform,
			});
		}
	}

	SceneNode::CollectDraws(context);
}

void Scene::Clear()
{
	_rootNodes.clear();
}

void Scene::AddRoot(const std::shared_ptr<SceneNode>& node)
{
	if (node) {
		_rootNodes.push_back(node);
	}
}

void Scene::RefreshTransforms()
{
	for (const auto& node : _rootNodes) {
		node->RefreshTransform(glm::mat4{ 1.0f });
	}
}

DrawContext Scene::BuildDrawContext() const
{
	DrawContext context;

	for (const auto& node : _rootNodes) {
		node->CollectDraws(context);
	}

	return context;
}
