#include "SceneRenderer.h"

#include "runtime/framework/Scene.h"
#include "runtime/framework/Node.h"
#include "runtime/framework/components/MeshComponent.h"

namespace shzk
{
	void SceneRenderer::Process(const std::shared_ptr<Scene>& scene)
	{
		const auto& nodes = scene->GetNodes();
		for (const auto& node : nodes)	ProcessNode(node);
	}

	void SceneRenderer::ProcessNode(const std::shared_ptr<Node>& node)
	{
		for (const auto& child : node->GetChildren())
		{
			std::shared_ptr<MeshComponent> mesh = child->TryGetComponent<MeshComponent>();
			if (mesh)
			{

			}
				 
			ProcessNode(child);
		}
	}
}