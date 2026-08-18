#include "MeshPassProcessor.h"

namespace shzk
{
	void MeshPassProcessor::Init()
	{

	}

	void MeshPassProcessor::Process(const std::vector<MeshBatch>& batches)
	{
		// 1. Clear
		m_oneFrameMeshDrawCommands.clear();

		// 2. Collect Mesh Batches
		for (const MeshBatch& batch : batches)
		{
			AddMeshBatch(batch);
		}
	}

	void MeshPassProcessor::BuildMeshDrawCommands(const MeshBatch& batch)
	{
	}
}