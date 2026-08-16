#include "ForwardPass.h"
#include "MeshPass.h"
#include "MeshPassProcessor.h"

#include <memory>

namespace shzk
{
// ForwardPass
	void ForwardPass::Init()
	{
		m_meshPassProcessor = std::make_shared<ForwardPassProcessor>(this);
		MeshPass::Init();	// mesh pass processor init


	}

// ForwardPassProcessor
	void ForwardPassProcessor::Init()
	{
		MeshPassProcessor::Init();

		// nothing else (?)
	}		

	void ForwardPassProcessor::AddMeshBatch(const MeshBatch& batch)
	{
		// TODO: RenderPass Mask

	}
}