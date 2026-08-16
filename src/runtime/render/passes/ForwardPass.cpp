#include "ForwardPass.h"
#include "MeshPass.h"
#include "MeshPassProcessor.h"

#include "runtime/asset/Shader.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <memory>

namespace shzk
{
// ForwardPass
	void ForwardPass::Init()
	{
		m_meshPassProcessor = std::make_shared<ForwardPassProcessor>(this);
		MeshPass::Init();	// mesh pass processor init

		m_vertexShader = std::make_shared<Shader>(SHZK_SPIRV_DIR "forward.vert.spv", SHADER_FREQUENCY_VERTEX, "main");
		m_fragShader = std::make_shared<Shader>(SHZK_SPIRV_DIR "forward.frag.spv", SHADER_FREQUENCY_FRAGMENT, "main");

		// 0817 RHIRootSignature Init
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