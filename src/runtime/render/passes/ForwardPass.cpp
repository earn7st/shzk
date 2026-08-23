#include "ForwardPass.h"
#include "MeshPass.h"
#include "MeshPassProcessor.h"

#include "runtime/asset/Shader.h"
#include "runtime/asset/Material.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <memory>

namespace shzk
{
// ForwardPass
	void ForwardPass::Init()
	{
		m_meshPassProcessor = std::make_shared<ForwardPassProcessor>(this);

		m_vertexShader = std::make_shared<Shader>(SHZK_SPIRV_DIR "forward.vert.spv", SHADER_FREQUENCY_VERTEX, "main");
		m_fragmentShader = std::make_shared<Shader>(SHZK_SPIRV_DIR "forward.frag.spv", SHADER_FREQUENCY_FRAGMENT, "main");

		// TODO
		RHIRootSignatureInfo info{};
		m_rootSignature = RHI::Get()->CreateRootSignature(info);
		
	}

// ForwardPassProcessor	

	ForwardPassProcessor::ForwardPassProcessor(ForwardPass* pass)
		: MeshPassProcessor(), m_pass(pass)
	{
		for (auto& rt : m_renderState.blendState.renderTargets)
		{
			rt.bEnable = false;
			rt.colorWriteMask	= COLOR_WRITE_MASK_RGBA;
			rt.colorBlendOp		= BlendOp::Add;
			rt.alphaBlendOp		= BlendOp::Add;
			rt.colorSrcBlend	= BlendFactor::One;
			rt.colorDstBlend	= BlendFactor::Zero;
			rt.alphaSrcBlend	= BlendFactor::One;
			rt.alphaDstBlend	= BlendFactor::Zero;
		}

		m_renderState.depthStencilState.bEnableDepthTest = true;	// default value, could be override by material parameters
		m_renderState.depthStencilState.bEnableDepthWrite = true;
		m_renderState.depthStencilState.depthTest = CompareFunction::LessEqual;

		// m_renderState.stencilRef = 0;
	}

	void ForwardPassProcessor::AddMeshBatch(const MeshBatch& batch)
	{
		std::shared_ptr<Material> material = batch.material;
		if (!material) return;
		if (material->GetPassMask() & PASS_MASK_FORWARD_PASS)
		{
			std::shared_ptr<Shader> vertexShader = material->GetVertexShader();
			std::shared_ptr<Shader> fragmentShader = material->GetFragmentShader();

			MeshPassProcessorRenderState renderState = m_renderState;	// baseline
			renderState.depthStencilState.bEnableDepthTest	= material->DepthTest();
			renderState.depthStencilState.bEnableDepthWrite = material->DepthWrite();
			renderState.depthStencilState.depthTest			= material->GetDepthCompare();

			BuildMeshDrawCommands(
				batch,
				material,
				renderState,
				vertexShader != nullptr ? vertexShader->m_shader : m_pass->GetVertexShader()->m_shader,
				fragmentShader != nullptr ? fragmentShader->m_shader : m_pass->GetFragmentShader()->m_shader,
				material->GetRasterizerCullMode(),
				material->GetRasterizerFillMode());
		}
	}
	RHIGraphicsPipelineInfo ForwardPassProcessor::BuildRHIGraphicsPipelineInfo(const GraphicsMinimalPipelineState& minimal)
	{
		RHIGraphicsPipelineInfo info{};
		info.vertexShader = minimal.boundShaderStateInput.vertexShader;
		info.fragmentShader = minimal.boundShaderStateInput.fragmentShader;
		info.rootSignature = m_pass->GetRHIRootSignature();
		info.vertexInputState = minimal.boundShaderStateInput.declaration;
		info.primitiveType = minimal.primitiveType;
		info.rasterizerState = minimal.rasterizerState;
		info.blendState = minimal.blendState;
		info.depthStencilState = minimal.depthStencilState;
		info.colorAttachmentFormats = m_pass->GetColorAttachmentFormats();
		info.depthStencilAttachmentFormat = m_pass->GetDepthStencilAttachmentFormat();
		info.viewMask = m_pass->GetViewMask();

		return info;
	}
}