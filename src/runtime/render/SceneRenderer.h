#pragma once

#include <memory>
#include <vector>

namespace shzk
{
	class Scene;

	class SceneRenderer
	{
	public:
		void Process(const std::shared_ptr<Scene>& scene);
		void CollectNodeMesh(const std::shared_ptr<Node>& node, std::vector<MeshBatch>& batches);
	};
}