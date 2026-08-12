#include "MeshComponent.h"

#include "runtime/asset/Model.h"
#include "runtime/core/Transform.h"
#include "runtime/render/resources/Buffer.h"

namespace shzk
{
	void MeshComponent::CollectMeshBatch(std::vector<MeshBatch>& batches)
	{
		auto& submeshes = m_model->GetSubmeshes();
		
		for (auto& submesh : submeshes)
		{
			MeshBatchElement element {
				.vertexBuffer		= submesh.vertexBuffer,
				.indexBuffer		= submesh.indexBuffer,
				.firstIndex			= 0,
				.indexCount			= submesh.indexBuffer->GetIndexNum(),
				.baseVertexIndex = 0,
				.modelMatrix		= submesh.localTransform.ToMat4x4(),
			};

			batches.emplace_back(element, submesh.material);
		}
	}
}