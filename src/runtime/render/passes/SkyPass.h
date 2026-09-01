#pragma once

#include "MeshPass.h"
#include "MeshPassProcessor.h"

namespace shzk
{
	class SkyPass : public MeshPass
	{
	public:
		SkyPass() : MeshPass(PassType::Sky) {}
		~SkyPass() = default;

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

		std::array<RHIFormat, MAX_RENDER_TARGETS>	m_colorAttachmentFormats = { FORMAT_UKNOWN };
		RHIFormat									m_depthStencilAttachmentFormat = FORMAT_UKNOWN;
		uint32_t m_viewMask = 0b00000000;		// single view

		RHIRenderPassInfo m_renderPassInfo{};
	};

	class SkyPassProcessor : public MeshPassProcessor
	{
	public:
		SkyPassProcessor() = delete;
		SkyPassProcessor(SkyPass* pass);
		~SkyPassProcessor() = default;

	protected:
		virtual void AddMeshBatch(const MeshBatch& batch) override final;
		virtual RHIGraphicsPipelineInfo BuildRHIGraphicsPipelineInfo(const GraphicsMinimalPipelineState& minimal) override final;

	private:
		SkyPass* m_pass = nullptr;
	};
}