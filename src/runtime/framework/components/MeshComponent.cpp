#include "MeshComponent.h"

#include "runtime/asset/Model.h"
#include "runtime/core/Transform.h"
#include "runtime/render/resources/Buffer.h"

namespace shzk
{
	void MeshComponent::CollectMeshBatchWithTransform(std::vector<MeshBatch>& batches, const glm::mat4x4& accTransformMat)
	{
		std::vector<MeshBatchElement> elements{};
		
		auto& submeshes = m_model->GetSubmeshes();
		for (auto& submesh : submeshes)
		{
			MeshBatchElement element{
				.indexBuffer		= submesh.indexBuffer,
				.firstIndex			= 0,
				.indexCount			= submesh.indexBuffer->GetIndexNum(),
				.modelMatrix		= submesh.localTransform.ToMat4x4() * accTransformMat,
			};
			elements.push_back(element);
			batches.emplace_back(elements, submesh.vertexFactory, submesh.material, submesh.primitiveType);	// only one MeshBatchElement per MeshBatch for now
		}
	}
}