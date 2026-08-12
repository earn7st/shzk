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
		// virtual void CollectMeshBatch(std::vector<MeshBatch>& meshBatches) = 0;	// TODO

		// for now, use push constants to uplaod per object transform
		// so transform should be collected here
		virtual void CollectMeshBatch(std::vector<MeshBatch>& meshBatches, Transform& outTransform) = 0;		

	};
}