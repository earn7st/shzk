#include "SceneRenderer.h"
#include "MeshDrawCommand.h"

#include "runtime/framework/Scene.h"
#include "runtime/framework/Node.h"
#include "runtime/framework/components/MeshComponent.h"

namespace shzk
{
	void SceneRenderer::Process(const std::shared_ptr<Scene>& scene)
	{
		std::vector<MeshBatch>	batches;
		const auto& nodes = scene->GetNodes();
		for (const auto& node : nodes)	CollectNodeMesh(node, batches);
	}

	void SceneRenderer::CollectNodeMesh(const std::shared_ptr<Node>& node, std::vector<MeshBatch>& batches)
	{
		for (const auto& child : node->GetChildren())
		{
			std::shared_ptr<MeshComponent> mesh = child->TryGetComponent<MeshComponent>();
			if (mesh)
			{
				mesh->CollectMeshBatch()
			}
				 
			CollectNodeMesh(child, batches);
		}
	}
}