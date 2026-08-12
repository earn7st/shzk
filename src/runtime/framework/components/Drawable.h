#pragma once

#include "runtime/render/resources/MeshBatch.h"

#include <vector>

namespace shzk
{
	// Drawable
	// Put it here just because only MeshComponent and SkyboxComponent inherited it
	class Drawable
	{
	public:
		virtual void CollectMeshBatch(std::vector<MeshBatch>& meshBatches) = 0;
	};
}