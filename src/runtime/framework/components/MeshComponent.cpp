#include "MeshComponent.h"

#include "runtime/asset/Model.h"
#include "runtime/core/Transform.h"
#include "runtime/render/resources/Buffer.h"

namespace shzk
{
	void MeshComponent::CollectMeshBatchWithTransform(std::vector<MeshBatch>& batches, const glm::mat4x4& accTransformMat)
	{
		auto& submeshes = m_model->GetSubmeshes();
		
		for (auto& submesh : submeshes)
		{
			MeshBatchElement element{
				.vertexFactory		= submesh.vertexFactory,
				.indexBuffer		= submesh.indexBuffer,
				.firstIndex			= 0,
				.indexCount			= submesh.indexBuffer->GetIndexNum(),
				.baseVertexIndex	= 0,
				.modelMatrix		= accTransformMat,
			};
			batches.emplace_back(element, submesh.material);
		}
	}
}