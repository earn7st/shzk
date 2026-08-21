#pragma once

#include "runtime/render/resources/RenderResourceDefinitions.h"
#include "runtime/rhi/RHIDefinitions.h"

#include <unordered_map>
#include <memory>
#include <string>

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

		void Init();

		std::shared_ptr<RHIShader> GetOrCreateRHIShader(const std::string& path, ShaderFrequency frequency, const std::string& entry = "main");
		std::shared_ptr<RHIDescriptorSet> CreateMaterialDescriptorSet();

	private:
		void InitGlobalResources();

		std::unordered_map<std::string, std::shared_ptr<RHIShader>> m_rhiShaderMap;

		struct PerFrameResource
		{
			std::shared_ptr<RHIDescriptorSet> descriptorSet;

			// Uniform Buffers: Camera, Light
		};
		std::array<PerFrameResource, FRAMES_IN_FLIGHT> m_perFrameResources;

		std::shared_ptr<RHIRootSignature> m_perFrameRootSignature;
		std::shared_ptr<RHIRootSignature> m_materialRootSignature;
	};
}