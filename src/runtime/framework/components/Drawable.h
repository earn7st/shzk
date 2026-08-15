#pragma once

#include "runtime/core/Transform.h"
#include "runtime/render/resources/MeshBatch.h"

#include <vector>

namespace shzk
{
	// Drawable
	// Put it here just because only MeshComponent and SkyboxComponent inherited it
	class Drawable
	{
	public:
		// virtual void CollectMeshBatch(std::vector<MeshBatch>& batches) = 0;
		virtual void CollectMeshBatchWithTransform(std::vector<MeshBatch>& batches, const glm::mat4x4& accTransformMat) = 0;
	};
}