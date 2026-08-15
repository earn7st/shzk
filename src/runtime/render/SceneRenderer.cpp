#include "SceneRenderer.h"
#include "MeshDrawCommand.h"
#include "runtime/render/resources/MeshBatch.h"
#include "runtime/framework/Scene.h"
#include "runtime/framework/Node.h"
#include "runtime/framework/components/MeshComponent.h"
#include "runtime/framework/components/TransformComponent.h"

#include <glm/glm.hpp>

namespace shzk
{
	void SceneRenderer::Process(const std::shared_ptr<Scene>& scene)
	{
		std::vector<MeshBatch>	batches;
		glm::mat4 transformMat = glm::identity<glm::mat4>();

		const auto& nodes = scene->GetNodes();
		for (const auto& node : nodes)	CollectNodeMesh(node, batches, transformMat);
	}

	void SceneRenderer::CollectNodeMesh(const std::shared_ptr<Node>& node, std::vector<MeshBatch>& batches, glm::mat4x4 accTransformMat)
	{
		std::shared_ptr<TransformComponent> transformComp = node->TryGetComponent<TransformComponent>();
		if (!transformComp) return;

		accTransformMat = transformComp->GetTransform().ToMat4x4() * accTransformMat;

		std::shared_ptr<MeshComponent>	meshComp = node->TryGetComponent<MeshComponent>();
		if (meshComp)
		{
			meshComp->CollectMeshBatchWithTransform(batches, accTransformMat);
		}

		// recursive
		for (const auto& child : node->GetChildren())
		{		 
			CollectNodeMesh(child, batches, accTransformMat);
		}
	}
}