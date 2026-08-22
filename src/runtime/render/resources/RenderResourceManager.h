#pragma once

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

		std::shared_ptr<GraphicsPipelineCache> GetGraphicsPipelineCache() const { return m_graphicsPipelineCache; }

		void Init();

		std::shared_ptr<RHIShader> GetOrCreateRHIShader(const std::string& path, ShaderFrequency frequency, const std::string& entry = "main");
		std::shared_ptr<RHIDescriptorSet> CreateMaterialDescriptorSet();

		const std::shared_ptr<Sampler> GetDefaultSampler() const { return m_samplers[0]; }

	private:
		void InitGlobalResources();

		// global Shader cache
		std::unordered_map<std::string, std::shared_ptr<RHIShader>> m_rhiShaderMap;

		// global GraphicsPipeline cache
		std::shared_ptr<GraphicsPipelineCache>	m_graphicsPipelineCache;

		// global root signature
		std::shared_ptr<RHIRootSignature> m_perFrameRootSignature;
		std::shared_ptr<RHIRootSignature> m_materialRootSignature;

		// per frame resources
		struct PerFrameResource
		{
			std::shared_ptr<RHIDescriptorSet> descriptorSet;

			// Uniform Buffers: Camera, Light
		};
		std::array<PerFrameResource, FRAMES_IN_FLIGHT> m_perFrameResources;

		// multiframe resources
		std::vector<std::shared_ptr<Sampler>> m_samplers;
	};
}