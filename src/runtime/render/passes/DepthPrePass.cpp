#include "DepthPrePass.h"

#include "MeshPass.h"
#include "MeshPassProcessor.h"

#include "runtime/asset/Shader.h"
#include "runtime/asset/Material.h"
#include "runtime/render/RenderConfig.h"
#include "runtime/render/resources/RenderResourceManager.h"
#include "runtime/rhi/RHI.h"
#include "runtime/rhi/RHICommandList.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <memory>

namespace shzk
{
	void DepthPrePass::Init()
	{
		m_meshPassProcessor = std::make_shared<DepthPrePassProcessor>(this);

		m_vertexShader = std::make_shared<Shader>(SHZK_SPIRV_DIR "default.vert.spv", SHADER_FREQUENCY_VERTEX, "main");
		m_fragmentShader = std::make_shared<Shader>(SHZK_SPIRV_DIR "depth_pre.frag.spv", SHADER_FREQUENCY_FRAGMENT, "main");

		{
			RHIRootSignatureInfo info{};
			auto perFrameRS = RenderResourceManager::Get()->GetPerFrameRootSignature();
			auto materialRS = RenderResourceManager::Get()->GetMaterialRootSignature();
			info.AddPushConstant({ .offset = 0, .size = 128, .frequency = SHADER_FREQUENCY_VERTEX });
			info.AddEntry(perFrameRS->GetInfo())
				.AddEntry(materialRS->GetInfo());
			m_rootSignature = RHI::Get()->CreateRootSignature(info);
		}

		{
			m_colorAttachmentFormats.fill(FORMAT_UKNOWN);
			m_depthStencilAttachmentFormat = DEPTH_FORMAT;
		}
	}

	void DepthPrePass::Prepare()
	{
		m_renderPassInfo = {};
		m_renderPassInfo.renderArea = RenderResourceManager::Get()->GetRenderExtent();
		m_renderPassInfo.layerCount = 1;
		m_renderPassInfo.viewMask = m_viewMask;

		std::shared_ptr<RHITextureView> sceneDepthView =
			RenderResourceManager::Get()->GetCurrentSceneDepthTextureView();
		
		auto& depth = m_renderPassInfo.depthStencilAttachment;
		depth.view = sceneDepthView;
		depth.layout = RHIResourceState::DepthStencilAttachment;
		depth.loadOp = AttachmentLoadOp::Clear;
		depth.storeOp = AttachmentStoreOp::Store;
		depth.clearDepth = 0.f;	// reverse-z
	}

	void DepthPrePass::Execute(std::shared_ptr<RHICommandList> cmd)
	{
		cmd->TextureBarrier({
			RenderResourceManager::Get()->GetCurrentSceneDepthTexture(),
			RHIResourceState::Undefined,
			RHIResourceState::DepthStencilAttachment
			});
		cmd->BeginRendering(m_renderPassInfo);
		cmd->SetViewport({ 0, 0 }, { m_renderPassInfo.renderArea.width, m_renderPassInfo.renderArea.height });
		cmd->SetScissor({ 0, 0 }, { m_renderPassInfo.renderArea.width, m_renderPassInfo.renderArea.height });
		m_meshPassProcessor->ExecuteDrawCommands(cmd);
		cmd->EndRendering();
	}

	// DepthPrePass Processor
	DepthPrePassProcessor::DepthPrePassProcessor(DepthPrePass* pass)
		: MeshPassProcessor(), m_pass(pass)
	{
		for (auto& rt : m_renderState.blendState.renderTargets)
		{
			rt.bEnable = false;
			rt.colorWriteMask = COLOR_WRITE_MASK_RGBA;
			rt.colorBlendOp = BlendOp::Add;
			rt.alphaBlendOp = BlendOp::Add;
			rt.colorSrcBlend = BlendFactor::One;
			rt.colorDstBlend = BlendFactor::Zero;
			rt.alphaSrcBlend = BlendFactor::One;
			rt.alphaDstBlend = BlendFactor::Zero;
		}

		m_renderState.depthStencilState.bEnableDepthTest = true;	// default value, could be override by material parameters
		m_renderState.depthStencilState.bEnableDepthWrite = true;
		m_renderState.depthStencilState.depthTest = CompareFunction::GreaterEqual; // reverse-z

		// m_renderState.stencilRef = 0;
	}

	void DepthPrePassProcessor::AddMeshBatch(const MeshBatch& batch)
	{
		std::shared_ptr<Material> material = batch.material;
		if (!material) return;
		if (material->GetPassMask() & PASS_MASK_DEPTH_PRE_PASS)
		{
			std::shared_ptr<Shader> vertexShader = material->GetVertexShader();
			std::shared_ptr<Shader> fragmentShader = material->GetFragmentShader();

			MeshPassProcessorRenderState renderState = m_renderState;	// baseline
			// renderState.depthStencilState.bEnableDepthTest = material->DepthTest();
			// renderState.depthStencilState.bEnableDepthWrite = material->DepthWrite();
			// renderState.depthStencilState.depthTest = material->GetDepthCompare();

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

	RHIGraphicsPipelineInfo DepthPrePassProcessor::BuildRHIGraphicsPipelineInfo(const GraphicsMinimalPipelineState& minimal)
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
