#pragma once

#include "VKCommon.h"
#include "assets/AssetTypes.h"

struct RenderObject
{
	std::shared_ptr<MeshAsset> mesh;
	uint32_t surfaceIndex{ 0 };
	glm::mat4 transform{ 1.0f };
};

struct DrawContext
{
	std::vector<RenderObject> renderObjects;
};

class SceneNode : public std::enable_shared_from_this<SceneNode>
{
public:
	std::string name;
	glm::mat4 localTransform{ 1.0f };
	glm::mat4 worldTransform{ 1.0f };
	std::weak_ptr<SceneNode> parent;
	std::vector<std::shared_ptr<SceneNode>> children;

	virtual ~SceneNode() = default;

	void AddChild(const std::shared_ptr<SceneNode>& child);
	void RefreshTransform(const glm::mat4& parentTransform);
	virtual void CollectDraws(DrawContext& context) const;
};

class MeshNode : public SceneNode
{
public:
	std::shared_ptr<MeshAsset> mesh;

	void CollectDraws(DrawContext& context) const override;
};

class Scene
{
public:
	void Clear();
	void AddRoot(const std::shared_ptr<SceneNode>& node);
	void RefreshTransforms();
	DrawContext BuildDrawContext() const;

	const std::vector<std::shared_ptr<SceneNode>>& RootNodes() const { return _rootNodes; }

private:
	std::vector<std::shared_ptr<SceneNode>> _rootNodes;
};
