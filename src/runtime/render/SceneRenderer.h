#pragma once

#include <memory>
#include <vector>
#include <glm/glm.hpp>

namespace shzk
{
	class Scene;
	class Node;
	struct MeshBatch;

	class SceneRenderer
	{
	public:
		void Process(const std::shared_ptr<Scene>& scene);

	private:
		void CollectNodeMesh(const std::shared_ptr<Node>& node, std::vector<MeshBatch>& batches, glm::mat4x4 accTransformMat);
	};
}