#pragma once

#include "MeshPass.h"
#include "MeshPassProcessor.h"
#include "runtime/asset/Shader.h"

namespace shzk
{
	class ForwardPass : public MeshPass
	{
	public:
		ForwardPass() : MeshPass(PassType::Forward) {}
		~ForwardPass() = default;

		virtual void Init() override final;
		virtual void Prepare() override final;
		virtual void Execute(std::shared_ptr<RHICommandList> cmd) override final;

		std::shared_ptr<Shader> GetVertexShader() const { return m_vertexShader; }
		std::shared_ptr<Shader> GetFragmentShader() const { return m_fragmentShader; }
		std::shared_ptr<RHIRootSignature> GetRHIRootSignature() const { return m_rootSignature; }
		const std::array<RHIFormat, MAX_RENDER_TARGETS>& GetColorAttachmentFormats() const { return m_colorAttachmentFormats; }
		RHIFormat GetDepthStencilAttachmentFormat() const { return m_depthStencilAttachmentFormat; }
		uint32_t GetViewMask() const { return m_viewMask; }

	private:
		std::shared_ptr<Shader>	m_vertexShader;
		std::shared_ptr<Shader> m_fragmentShader;

		std::shared_ptr<RHIRootSignature> m_rootSignature;			

		// TODO: Encapsulation of RenderTargets
		std::array<RHIFormat, MAX_RENDER_TARGETS>	m_colorAttachmentFormats = { FORMAT_UKNOWN };
		RHIFormat									m_depthStencilAttachmentFormat = FORMAT_UKNOWN;
		uint32_t m_viewMask = 0b00000000;

		RHIRenderPassInfo m_renderPassInfo{};
	};

	class ForwardPassProcessor : public MeshPassProcessor
	{
	public:
		ForwardPassProcessor() = delete;
		ForwardPassProcessor(ForwardPass* pass);
		~ForwardPassProcessor() = default;
		
	protected:
		virtual void AddMeshBatch(const MeshBatch& batch) override final;
		virtual RHIGraphicsPipelineInfo BuildRHIGraphicsPipelineInfo(const GraphicsMinimalPipelineState& minimal) override final;
		
	private:
		ForwardPass* m_pass;
	};
}