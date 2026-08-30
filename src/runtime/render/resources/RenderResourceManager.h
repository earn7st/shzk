#pragma once

#include "runtime/render/resources/Buffer.h"
#include "runtime/render/resources/Sampler.h"
#include "runtime/render/resources/GraphicsPipelineCache.h"
#include "runtime/render/resources/RenderResourceDefinitions.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <unordered_map>
#include <memory>
#include <string>
#include <vector>

namespace shzk
{

	class GraphicsPipelineState;
	class RHIShader;

	class RenderResourceManager
	{
	public:
		static std::shared_ptr<RenderResourceManager> g_renderResourceManager;

	public:
		RenderResourceManager() = default;
		~RenderResourceManager() = default;

		static std::shared_ptr<RenderResourceManager>& Get() { return g_renderResourceManager; }
		static std::shared_ptr<GraphicsPipelineCache>& GetGraphicsPipelineCache() { return g_renderResourceManager->m_graphicsPipelineCache; }

		void Init();
		void BeginFrame(uint32_t frameIdx);

		std::shared_ptr<RHIShader> GetOrCreateRHIShader(const std::string& path, ShaderFrequency frequency, const std::string& entry = "main");

		std::shared_ptr<RHIRootSignature> GetPerFrameRootSignature() const { return m_perFrameRootSignature; }
		std::shared_ptr<RHIRootSignature> GetMaterialRootSignature() const { return m_materialRootSignature; }
		std::shared_ptr<RHIDescriptorSet> CreateMaterialDescriptorSet();

		// per frame resources
		std::shared_ptr<RHIDescriptorSet> GetCurrentPerFrameDescriptorSet() const { return m_perFrameResources[m_frameIndex].descriptorSet; }
		std::shared_ptr<Buffer<PerFrameUniformShaderParameters>> GetCurrentPerFrameUniformBuffer() const{ return m_perFrameResources[m_frameIndex].ub; }
		std::shared_ptr<RHITexture>	GetCurrentSceneColorTexture() const { return m_perFrameResources[m_frameIndex].sceneColorTexture; }
		std::shared_ptr<RHITextureView> GetCurrentSceneColorTextureView() const { return m_perFrameResources[m_frameIndex].sceneColorTextureView; }
		std::shared_ptr<RHITexture> GetCurrentSceneDepthTexture() const { return m_perFrameResources[m_frameIndex].sceneDepthTexture; }
		std::shared_ptr<RHITextureView> GetCurrentSceneDepthTextureView() const { return m_perFrameResources[m_frameIndex].sceneDepthTextureView; }

		const std::shared_ptr<Sampler> GetDefaultSampler() const { return m_samplers[0]; }

		inline Extent2D GetRenderExtent() const { return m_renderExtent; }
		inline void SetRenderExtent(Extent2D extent) { m_renderExtent = extent; }

	private:
		void InitGlobalResources();

		// global Shader cache
		std::unordered_map<std::string, std::shared_ptr<RHIShader>> m_rhiShaderMap;

		// global GraphicsPipeline cache
		std::shared_ptr<GraphicsPipelineCache>	m_graphicsPipelineCache = std::make_shared<GraphicsPipelineCache>();

		// global root signature
		std::shared_ptr<RHIRootSignature> m_perFrameRootSignature;	// as true source of set 0
		std::shared_ptr<RHIRootSignature> m_materialRootSignature;	// as true source of set 1

		// per frame resources
		struct PerFrameResource
		{
			// Shader Binding
			std::shared_ptr<RHIDescriptorSet> descriptorSet;
			std::shared_ptr<Buffer<PerFrameUniformShaderParameters>> ub;

			// Render Targets
			std::shared_ptr<RHITexture>	sceneColorTexture;
			std::shared_ptr<RHITextureView> sceneColorTextureView;
			std::shared_ptr<RHITexture> sceneDepthTexture;
			std::shared_ptr<RHITextureView> sceneDepthTextureView;

		};
		std::array<PerFrameResource, FRAMES_IN_FLIGHT> m_perFrameResources;
		uint32_t m_frameIndex = 0;

		// multiframe resources
		std::vector<std::shared_ptr<Sampler>> m_samplers;

		// render extent
		Extent2D m_renderExtent{1280, 720};
	};
}